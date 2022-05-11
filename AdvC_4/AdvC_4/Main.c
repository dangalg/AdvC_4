#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h> //uncomment this block to check for heap memory allocation leaks.
// Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define MAX_LINE_LENGTH 1023
#define FACTOR_RANGE 20

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

void releaseMemory(char*** students, int* coursesPerStudent, int numberOfStudents);
void releaseStructMemory(Student* transformedStudents, int numberOfStudents);


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
	Student* transformedStudents = transformStudentArray(students, coursesPerStudent, numberOfStudents);
	writeToBinFile("students1.bin", transformedStudents, numberOfStudents);
	Student* testReadStudents = readFromBinFile("students.bin");

	//add code to free all arrays of struct Student
	releaseMemory(students, coursesPerStudent, numberOfStudents);
	releaseStructMemory(transformedStudents, numberOfStudents);
	releaseStructMemory(testReadStudents, numberOfStudents);

	_CrtDumpMemoryLeaks(); //uncomment this block to check for heap memory allocation leaks.
	// Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019
	return 0;
}

void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
	// open file for reading
	FILE* fp = fopen(fileName, "r");
	if (fp == NULL) { printf("Can't open file\n"); exit(1); }
	rewind(fp);

	*numberOfStudents = 0;

	// set line max size
	char lineBuffer[MAX_LINE_LENGTH];

	// count students
	while (fgets(lineBuffer, MAX_LINE_LENGTH, fp))
	{
		(*numberOfStudents)++;
	}

	rewind(fp);

	// assign memory for course number for each student
	*(coursesPerStudent) = (int*)malloc(*numberOfStudents * sizeof(int));

	for (int i = 0; i < *numberOfStudents; i++)
	{
		// get next line
		fgets(lineBuffer, MAX_LINE_LENGTH, fp);

		// get course count for student i
		*(*(coursesPerStudent)+i) = countPipes(lineBuffer, MAX_LINE_LENGTH);
	}

	assert(fclose(fp)==0);
}

int countPipes(const char* lineBuffer, int maxCount)
{
	// if max count less than 0 return
	if (maxCount <= 0) { return 0; }

	// if line buffer not initialized return
	if (lineBuffer == NULL) { return -1; }

	int pipeCount = 0;

	// count pipes in one line
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
	if (!students) { printf("memory allocation error"); exit(1); }

	char lineBuffer[MAX_LINE_LENGTH];
	FILE* fp = fopen(fileName, "r");
	if (fp == NULL) { printf("Can't open file\n"); exit(1); }
	rewind(fp);

	for (int i = 0; i < *numberOfStudents; i++)
	{
		// get number of courses for student
		int numberOfCourses = (*(*(coursesPerStudent)+i)*2)+1;

		// assign dynamic space for current student courses
		*(students + i) = (char**)malloc(numberOfCourses * sizeof(char*));
		if (!(*(students + i))) { printf("memory allocation error"); exit(1); }

		// get line from file
		fgets(lineBuffer, MAX_LINE_LENGTH, fp);

		// remove \n from end of line
		char* pos;
		if ((pos = strchr(lineBuffer, '\n')) != NULL)
		{
			*pos = '\0';
		}
		else 
		{
			/* input too long for buffer, flag error */
		}
			

		char* token;
		const char s[2] = "|,";

		// get name
		token = strtok(lineBuffer, s);

		int j = 0;

		// assign space for student name
		*(*(students+i)+j) = (char*)malloc((strlen(token)+1) * sizeof(char));
		if (!(*(*(students + i) + j))) { printf("memory allocation error"); exit(1); }

		// insert name to array
		strcpy((*(*(students + i) + j)),token);

		j++;

		token = strtok(NULL, s);

		while(token != NULL)
		{
			// assign space for course
			*(*(students + i) + j) = (char*)malloc((strlen(token)+1) * sizeof(char));
			if (!(*(*(students + i) + j))) { printf("memory allocation error"); exit(1); }

			// insert course to array
			strcpy((*(*(students + i) + j)),token);

			j++;

			token = strtok(NULL, s);
		}

	}

	assert(fclose(fp)==0);

	return students;
}

void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor)
{
	//make sure factor is in correct range
	if (factor > FACTOR_RANGE || factor < -FACTOR_RANGE)
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
				strcpy(*(*(students + i) + j + 1) ,_itoa(grade, buffer, 10));
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
	// open file for writing
	FILE* fp = fopen("studentList1.txt", "w");
	if (fp == NULL) { printf("Can't open file\n"); exit(1); }
	rewind(fp);

	// loop through students
	for (int i = 0; i < numberOfStudents; i++)
	{
		// write name to file
		fputs(*(*(students + i)), fp);
		
		// write courses to file
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
		fputc('\n', fp);
	}

	assert(fclose(fp)==0);

	// release student memory after saving file
	releaseMemory(students, coursesPerStudent, numberOfStudents);
}

void writeToBinFile(const char* fileName, Student* students, int numberOfStudents)
{
	// open file for binary writing
	FILE* fp = fopen("studentList.bin", "wb");
	if (fp == NULL) { printf("Can't open file\n"); exit(1); }

	fwrite(&numberOfStudents, sizeof(int), 1, fp);

	for (int i = 0; i < numberOfStudents; i++)
	{
		// write student name
		fwrite(students[i].name, 35 * sizeof(char), 1, fp);
		
		// write number of courses
		fwrite(&students[i].numberOfCourses, sizeof(int), 1, fp);

		// get courses
		int numberOfCourses = students[i].numberOfCourses;
		for (int j = 0; j < students[i].numberOfCourses; j++)
		{
			// write course name
			fwrite(students[i].grades[j].courseName, 35 * sizeof(char), 1, fp);

			// write course grade
			fwrite(&students[i].grades[j].grade, sizeof(int), 1, fp);
		}
	}

	assert(fclose(fp)==0);
}

Student* readFromBinFile(const char* fileName)
{
	// open file for binary reading
	FILE* fp = fopen("studentList.bin", "rb");
	if (fp == NULL) { printf("Can't open file\n"); exit(1); }

	// get number of students
	int numberOfStudents = 0;
	fread(&numberOfStudents, sizeof(int), 1, fp);

	// assign memory for students struct
	Student* studentsStruct = (Student*)malloc(numberOfStudents * sizeof(Student));
	if (!studentsStruct) { printf("memory allocation error"); exit(1); }

	for (int i = 0; i < numberOfStudents; i++)
	{
		// get student name
		fread(studentsStruct[i].name, 35 * sizeof(char), 1, fp);

		// get student numbers of courses
		fread(&studentsStruct[i].numberOfCourses, sizeof(int), 1, fp);

		// assign memory for grades
		studentsStruct[i].grades = (StudentCourseGrade*)malloc(studentsStruct[i].numberOfCourses * sizeof(StudentCourseGrade));
		if (!studentsStruct[i].grades) { printf("memory allocation error"); exit(1); }

		// get courses
		int numberOfCourses = studentsStruct[i].numberOfCourses;
		for (int j = 0; j < studentsStruct[i].numberOfCourses; j++)
		{
			// write course name
			fread(studentsStruct[i].grades[j].courseName, 35 * sizeof(char), 1, fp);

			// write course grade
			fread(&studentsStruct[i].grades[j].grade, sizeof(int), 1, fp);
		}
	}

	assert(fclose(fp)==0);

	return studentsStruct;
}

Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents)
{
	// assign memory for students struct
	Student* studentsStruct = (Student*)malloc(numberOfStudents * sizeof(Student));
	if(!studentsStruct) { printf("memory allocation error"); exit(1); }

	for (int i = 0; i < numberOfStudents; i++)
	{
		// get student name
		strcpy(studentsStruct[i].name, *(*(students + i)));
		studentsStruct[i].numberOfCourses = *(coursesPerStudent + i);

		// assign memory for grades
		studentsStruct[i].grades = (StudentCourseGrade*)malloc(studentsStruct[i].numberOfCourses * sizeof(StudentCourseGrade));
		if (!studentsStruct[i].grades) { printf("memory allocation error"); exit(1); }
		// get courses
		int numberOfCourses = ((*(coursesPerStudent + i)) * 2);
		for (int j = 1, k=0; j < numberOfCourses; j+=2, k++)
		{
			strcpy(studentsStruct[i].grades[k].courseName, *(*(students + i) + j));
			studentsStruct[i].grades[k].grade = atoi(*(*(students + i) + j + 1));
		}
	}

	return studentsStruct;
}

void releaseMemory(char*** students, int* coursesPerStudent, int numberOfStudents)
{
	// release courses memory
	for (int i = 0; i < numberOfStudents; i++)
	{
		int numberOfCourses = ((*(coursesPerStudent + i))*2)+1;
		for (int j = 0; j < numberOfCourses; j++)
		{
			free(*(*(students + i) + j));
		}
	}

	// release students memory
	for (int i = 0; i < numberOfStudents; i++)
	{
		free(*(students + i));
	}

	// release courses number memory
	free(coursesPerStudent);

	// release student array
	free(students);
}

void releaseStructMemory(Student* transformedStudents, int numberOfStudents)
{
	for (int i = 0; i < numberOfStudents; i++)
	{
		// free memory for grades
		free(transformedStudents[i].grades);
	}

	// release student struct
	free(transformedStudents);
}