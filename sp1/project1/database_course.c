/*
 * Copyright (C) 2008	John Reese
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "project1.h"
#include "database.h"

#include <allocate.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

db_course* db_course_get( db_database* db, char* id ) {
	int cmp;
	db_course* course = db->courses;
	while ( course != NULL ) {
		cmp = strcmp( course->id, id );
		if ( cmp == 0 ) {
			break;
		}

		course = course->next;
	}

	return course;
}

int db_course_exists( db_database* db, char* id ) {
	db_course* course = db_course_get( db, id );

	return ( course != NULL );
}

int db_course_insert( db_database* db, char* id, int size ) {
	int cmp;

	// Find the appropriate spot in the list, and check for duplicate ID
	db_course* last = NULL;
	db_course* next = db->courses;
	while ( next != NULL ) {
		cmp = strcmp( id, next->id );
		if ( cmp == 0 ) {
			return DBERR_COURSE_EXISTS;
		} else if ( cmp < 1 ) {
			break;
		}

		last = next;
		next = next->next;
	}

	// Initialize new course
	db_course* course = db_allocate( sizeof( db_course ) );
	course->id = db_allocate( strlen( id ) );
	course->size = size;
	course->next = NULL;
	course->last = NULL;
	course->students = NULL;
	strcpy( course->id, id );

	// Insert new course into the list as appropriate
	course->last = last;
	course->next = next;
	if ( last != NULL ) {
		last->next = course;
	} else {
		db->courses = course;
	}
	if ( next != NULL ) {
		next->last = course;
	}

	return 0;
}

int db_course_remove( db_database* db, char* id ) {
	// Find the course in the database
	db_course* course = db_course_get( db, id );

	// Course not found
	if ( course == NULL ) {
		return DBERR_COURSE_NOT_EXISTS;
	}

	// Remove associated enrollments
	db_enrollment_remove_course( db, id );

	// Remove course from the list
	if ( course->last == NULL && course->next == NULL ) {
		db->courses = NULL;
	} else if ( course->last == NULL ) {
		db->courses = course->next;
		course->next->last = NULL;
	} else if ( course->next == NULL ) {
		course->last->next = NULL;
	} else {
		course->last->next = course->next;
		course->next->last = course->last;
	}

	unallocate( course->id );
	unallocate( course );
	return 0;
}

int db_course_count( db_database* db, char* id ) {
	int count = 0;
	db_course* course = db_course_get( db, id );

	if ( course == NULL ) {
		return -1;
	}

	db_enrollment* student = course->students;
	while ( student != NULL ) {
		count++;
		student = student->next_student;
	}

	return count;
}

int db_course_free( db_database* db, char* id ) {
	int count = db_course_count( db, id );

	if ( count < 0 ) {
		return -1;
	}

	db_course* course = db_course_get( db, id );
	return course->size - count;
}

