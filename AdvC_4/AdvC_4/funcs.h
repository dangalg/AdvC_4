#pragma once
#include <assert.h>

#define MAX_LINE_LENGTH 1023

typedef struct StudentCourseGrade
{
	char courseName[35];
	int grade;
}StudentCourseGrade;

typedef struct Student
{
	char name[35];
	StudentCourseGrade* grades; //dynamic array of courses
	int numberOfCourses;
}Student;

void releaseMemory(char*** students, int* coursesPerStudent, int numberOfStudents);
void releaseStructMemory(Student* transformedStudents, int numberOfStudents);