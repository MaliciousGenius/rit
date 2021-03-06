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

db_enrollment* db_enrollment_get( db_database* db, char* course_id, char* student_id ) {
	// Get student and course data
	db_course* course = db_course_get( db, course_id );
	db_student* student = db_student_get( db, student_id );

	// Either course or student not found
	if ( course == NULL || student == NULL ) {
		return NULL;
	}

	// Search the student's enrollments for course
	db_enrollment* enrollment = student->courses;
	while ( enrollment != NULL ) {
		if ( enrollment->course == course ) {
			break;
		}

		enrollment = enrollment->next_course;
	}

	return enrollment;
}

int db_enrollment_exists( db_database* db, char* course_id, char* student_id ) {
	db_enrollment* enrollment = db_enrollment_get( db, course_id, student_id );

	return ( enrollment != NULL );
}

int db_enrollment_insert( db_database* db, char* course_id, char* student_id ) {
	db_course* course;
	db_student* student;
	db_enrollment* enrollment;

	// Make sure the student exists
	student = db_student_get( db, student_id );
	if ( student == NULL ) {
		return DBERR_STUDENT_NOT_EXISTS;
	}

	// Make sure the course exists
	course = db_course_get( db, course_id );
	if ( course == NULL ) {
		return DBERR_COURSE_NOT_EXISTS;
	}

	// Make sure the student has a free slot
	if ( db_student_free( db, student_id ) < 1 ) {
		return DBERR_STUDENT_IS_FULL;
	}

	// Make sure the course has a free slot
	if ( db_course_free( db, course_id ) < 1 ) {
		return DBERR_COURSE_IS_FULL;
	}

	// Make sure the enrollment doesn't already exist
	enrollment = db_enrollment_get( db, course_id, student_id );
	if ( enrollment != NULL ) {
		return DBERR_STUDENT_ENROLLED;
	}

	// Now we can create the enrollment
	enrollment = db_allocate( sizeof( db_enrollment ) );
	enrollment->course = course;
	enrollment->student = student;
	enrollment->last_course = NULL;
	enrollment->last_student = NULL;

	// Insert the enrollment into the course and student lists
	db_enrollment_plonk( db, enrollment );

	return 0;
}

int db_enrollment_remove( db_database* db, char* course_id, char* student_id ) {
	db_course* course;
	db_student* student;
	db_enrollment* enrollment;

	// Make sure the student exists
	student = db_student_get( db, student_id );
	if ( student == NULL ) {
		return DBERR_STUDENT_NOT_EXISTS;
	}

	// Make sure the course exists
	course = db_course_get( db, course_id );
	if ( course == NULL ) {
		return DBERR_COURSE_NOT_EXISTS;
	}

	// Make sure the enrollment already exists
	enrollment = db_enrollment_get( db, course_id, student_id );
	if ( enrollment == NULL ) {
		return DBERR_STUDENT_NOT_ENROLLED;
	}

	// Remove the enrollment from course and student lists
	db_enrollment_pluck( db, enrollment );

	unallocate( enrollment );
	return 0;
}

int db_enrollment_remove_course( db_database* db, char* course_id ) {
	db_course* course;
	db_enrollment* enrollment;
	db_enrollment* next;

	// Make sure the course exists
	course = db_course_get( db, course_id );
	if ( course == NULL ) {
		return DBERR_COURSE_NOT_EXISTS;
	}

	// Remove all enrollments for the course
	enrollment = course->students;
	while ( enrollment != NULL ) {
		next = enrollment->next_student;

		db_enrollment_pluck( db, enrollment );
		unallocate( enrollment );

		enrollment = next;
	}

	return 0;
}

int db_enrollment_remove_student( db_database* db, char* student_id ) {
	db_student* student;
	db_enrollment* enrollment;
	db_enrollment* next;

	// Make sure the student exists
	student = db_student_get( db, student_id );
	if ( student == NULL ) {
		return DBERR_STUDENT_NOT_EXISTS;
	}

	// Remove all enrollments for the student
	enrollment = student->courses;
	while ( enrollment != NULL ) {
		next = enrollment->next_course;

		db_enrollment_pluck( db, enrollment );
		unallocate( enrollment );

		enrollment = next;
	}

	return 0;
}

int db_enrollment_plonk( db_database* db, db_enrollment* enrollment ) {
	int cmp;
	db_course* course = enrollment->course;
	db_student* student = enrollment->student;
	db_enrollment* last;
	db_enrollment* next;

	// Find the appropriate spot in the course list
	last = NULL;
	next = course->students;
	while ( next != NULL ) {
		cmp = strcmp( student->name, next->student->name );
		if ( cmp == 0 ) {
			cmp = strcmp( student->id, next->student->id );
			if ( cmp < 1 ) {
				break;
			}
		} else if ( cmp < 1 ) {
			break;
		}

		last = next;
		next = next->next_student;
	}

	// Insert the enrollment in the course list
	enrollment->last_student = last;
	enrollment->next_student = next;
	if ( last != NULL ) {
		last->next_student = enrollment;
	} else {
		course->students = enrollment;
	}
	if ( next != NULL ) {
		next->last_student = enrollment;
	}

	// Find the appropriate spot in the student list
	last = NULL;
	next = student->courses;
	while ( next != NULL ) {
		cmp = strcmp( course->id, next->course->id );
		if ( cmp < 1 ) {
			break;
		}

		last = next;
		next = next->next_course;
	}

	// Insert the enrollment in the student list
	enrollment->last_course = last;
	enrollment->next_course = next;
	if ( last != NULL ) {
		last->next_course = enrollment;
	} else {
		student->courses = enrollment;
	}
	if ( next != NULL ) {
		next->last_course = enrollment;
	}

	return 0;
}

int db_enrollment_pluck( db_database* db, db_enrollment* enrollment ) {
	db_course* course = enrollment->course;
	db_student* student = enrollment->student;
	db_enrollment* last;
	db_enrollment* next;

	// Remove the enrollment from the course list
	last = enrollment->last_student;
	next = enrollment->next_student;
	if ( last == NULL && next == NULL ) {
		course->students = NULL;
	} else if ( last == NULL ) {
		next->last_student = NULL;
		course->students = next;
	} else if ( next == NULL ) {
		last->next_student = NULL;
	} else {
		last->next_student = next;
		next->last_student = last;
	}

	// Remove the enrollment from the student list
	last = enrollment->last_course;
	next = enrollment->next_course;
	if ( last == NULL && next == NULL ) {
		student->courses = NULL;
	} else if ( last == NULL ) {
		next->last_course = NULL;
		student->courses = next;
	} else if ( next == NULL ) {
		last->next_course = NULL;
	} else {
		last->next_course = next;
		next->last_course = last;
	}

	return 0;
}

