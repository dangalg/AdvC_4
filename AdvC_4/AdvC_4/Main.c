#define _CRT_SECURE_NO_WARNINGS
/*#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>*/ //uncomment this block to check for heap memory allocation leaks.
// Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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


//Part A
void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
int countPipes(const char* lineBuffer, int maxCount);
char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
void printStudentArray(const char* const* const* students, const int* coursesPerStudent, int numberOfStudents);
void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor);
void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents);

//Part B
Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents);
void writeToBinFile(const char* fileName, Student* students, int numberOfStudents);
Student* readFromBinFile(const char* fileName);

int main()
{
	//Part A
	int* coursesPerStudent = NULL;
	int numberOfStudents = 0;
	char*** students = makeStudentArrayFromFile("studentList.txt", &coursesPerStudent, &numberOfStudents);
	factorGivenCourse(students, coursesPerStudent, numberOfStudents, "Advanced Topics in C", +5);
	printStudentArray(students, coursesPerStudent, numberOfStudents);
	//studentsToFile(students, coursesPerStudent, numberOfStudents); //this frees all memory. Part B fails if this line runs. uncomment for testing (and comment out Part B)

	//Part B
	//Student* transformedStudents = transformStudentArray(students, coursesPerStudent, numberOfStudents);
	//writeToBinFile("students.bin", transformedStudents, numberOfStudents);
	//Student* testReadStudents = readFromBinFile("students.bin");

	//add code to free all arrays of struct Student


	/*_CrtDumpMemoryLeaks();*/ //uncomment this block to check for heap memory allocation leaks.
	// Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019

	return 0;
}

void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
	FILE* fp = fopen(fileName, "r");
	assert(fp);
	rewind(fp);

	*numberOfStudents = 0;
	char lineBuffer[MAX_LINE_LENGTH];

	while (fgets(lineBuffer, MAX_LINE_LENGTH, fp))
	{
		(*numberOfStudents)++;
	}

	rewind(fp);
	*(coursesPerStudent) = (int*)malloc(*numberOfStudents * sizeof(int));

	for (int i = 0; i < *numberOfStudents; i++)
	{
		fgets(lineBuffer, MAX_LINE_LENGTH, fp);
		*(*(coursesPerStudent)+i) = countPipes(lineBuffer, MAX_LINE_LENGTH);
	}

	fclose(fp);
}

int countPipes(const char* lineBuffer, int maxCount)
{
	if (maxCount <= 0) { return 0; }
	if (lineBuffer == NULL) { return -1; }

	int pipeCount = 0;

	for (int i	= 0; i < maxCount && *lineBuffer != '\0'; i++)
	{
		if (*lineBuffer == '|')
		{
			pipeCount++;
		}

		lineBuffer++;
	}

	return pipeCount;
}

char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
	countStudentsAndCourses(fileName, coursesPerStudent, numberOfStudents);

	// assign dynamic space for all students
	char*** students = (char***)malloc((*numberOfStudents) * sizeof(char**));

	char lineBuffer[MAX_LINE_LENGTH];
	FILE* fp = fopen(fileName, "r");
	assert(fp);
	rewind(fp);

	for (int i = 0; i < *numberOfStudents; i++)
	{
		// number of courses
		int numberOfCourses = (*(*(coursesPerStudent)+i)*2)+1;

		// assign dynamic space for current student
		*(students + i) = (char**)malloc(numberOfCourses * sizeof(char*));

		// get line from file
		fgets(lineBuffer, MAX_LINE_LENGTH, fp);

		char* token;
		const char s[2] = "|,";

		// get name
		token = strtok(lineBuffer, s);

		int j = 0;

		// assign space for student name
		*(*(students+i)+j) = (char*)malloc((strlen(token)+1) * sizeof(char));

		// insert name to array
		strcpy((*(*(students + i) + j)),token);

		j++;

		token = strtok(NULL, s);

		while(token != NULL)
		{
			// assign space for course
			*(*(students + i) + j) = (char*)malloc((strlen(token)+1) * sizeof(char));

			// insert course to array
			strcpy((*(*(students + i) + j)),token);

			j++;

			token = strtok(NULL, s);
		}

	}

	fclose(fp);

	return students;
}

void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor)
{
	if (factor > 20 || factor < -20) 
	{
		printf("Factor out of range must be between -20 to 20");
		return; 
	}

	for (int i = 0; i < numberOfStudents; i++)
	{
		int numberOfCourses = (*(coursesPerStudent + i) * 2);
		for (int j = 1; j < numberOfCourses; j++)
		{
			char* currentCourse = *(*(students + i) + j);

			// if course name found
			if (strcmp(currentCourse, courseName) == 0)
			{
				// plus 1 to get to grade
				// convert grade to int
				int grade = atoi(*(*(students + i) + j + 1));
				// add factor
				grade += factor;
				char* buffer[3];
				// return factored grade to list
				*(*(students + i) + j + 1) = _itoa(grade, buffer, 10);
			}
		}
	}
}

void printStudentArray(const char* const* const* students, const int* coursesPerStudent, int numberOfStudents)
{
	for (int i = 0; i < numberOfStudents; i++)
	{
		printf("name: %s\n*********\n", students[i][0]);
		for (int j = 1; j <= 2 * coursesPerStudent[i]; j += 2)
		{
			printf("course: %s\n", students[i][j]);
			printf("grade: %s\n", students[i][j + 1]);
		}
		printf("\n");
	}
}

void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents)
{
	FILE* fp = fopen("studentList.txt", "w");
	assert(fp);
	rewind(fp);

	for (int i = 0; i < numberOfStudents; i++)
	{
		fputs(*(*(students + i)), fp);
		
		int numberOfCourses = (*(coursesPerStudent + i) * 2);
		for (int j = 1; j < numberOfCourses + 1; j+=2)
		{
			char* currentCourse = *(*(students + i) + j);
			char* currentGrade = *(*(students + i) + j + 1);
			
			fputc('|', fp);
			fputs(currentCourse, fp);
			fputc(',', fp);
			fputs(currentGrade, fp);
		}
	}

	fclose(fp);
}

void writeToBinFile(const char* fileName, Student* students, int numberOfStudents)
{
	//add code here
}

Student* readFromBinFile(const char* fileName)
{
	//add code here
}

Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents)
{
	//add code here
}

char* replace_char(char* str, char find, char replace) {
	char* current_pos = strchr(str, find);
	while (current_pos) {
		*current_pos = replace;
		current_pos = strchr(current_pos, find);
	}
	return str;
}
