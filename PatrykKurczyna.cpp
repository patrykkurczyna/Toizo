#include <stdio.h>
#include <iostream>
#include <vector>
#include <list>
#include <cstdio>
#include <stdlib.h>
#include <string.h>  
#include <strings.h>  

#define BUFLEN 100

using namespace std;

typedef struct Point{
	int x;
	int y;
} Point;

typedef struct Robot{
	struct Point *points;
	Point ** combinations;
	int possibilities;
	int pathLength;
	int numberOfSlides;
	int totalFieldsCovered;
	int id;
	int x_max;
	int x_min;
	int y_max;
	int y_min;
	// needed when connecting two or more robots
	int *ids;
	int idIndex;
	int merged;
} Robot;

//global variables
int fieldsToCover = 0;
int numberOfRobots = 0;
int width = 0;
int height = 0;
int idMax = 0;
int coverageMax = 0;

int containsId(int id, int * ids, int limit){
	int i = 0;

	for(i = 0; i < limit; i++){
		if(id == ids[i]){
			return 1;
		}
	}
	return 0;
}

//Checks if given coordinates are already in coords array - returns 1 if so
int robotContainsThisPoint(Point point, Point * points, int length){
	int i = 0;
	for(i = 0; i < length ; i++){
		if(points[i].x == point.x && points[i].y == point.y){
			return 0;
		}
	}
	return 1;
}

struct Robot parseInputForRobot() {
	Robot robot;
	
	cin >> robot.id;
    cin >> robot.numberOfSlides;
    char direction;
    int slideLength;
    Point point;
	point.x = 0;
	point.y = 0;
	robot.points = (Point *) malloc(width * height * sizeof * robot.points);
	int countSteps = 1;
	int totalFieldsCovered = 1;
	int y_max = 0;
	int y_min = 0;
	int x_max = 0;
	int x_min = 0;
	int index = 0;
	robot.points[index] = point;
	index++;
	
    for(int i=0; i<robot.numberOfSlides; i++) {
        cin >> direction;
        cin >> slideLength;

        switch(direction) {
            case 'S':
                for(int j = 0; j<slideLength; j++) {
                    point.y += 1;
					if( robotContainsThisPoint(point, robot.points, countSteps) ){
						//cout << point.x << "_____" << point.y << "\n";
						robot.points[index] = point;
						//cout << robot.points[index].x << "====" << robot.points[index].y << "\n";
						index++;
						totalFieldsCovered++;
						if(y_max < point.y ){
							y_max = point.y;
						}
					}
				}
				countSteps += slideLength;
                break;
            case 'N':
                for(int j = 0; j<slideLength; j++){
                    point.y -= 1;
					if( robotContainsThisPoint(point, robot.points, countSteps) ){
					//cout << point.x << "_____" << point.y << "\n";
						robot.points[index] = point;
						//cout << robot.points[index].x << "====" << robot.points[index].y << "\n";
						index++;
						totalFieldsCovered++;
						if(y_min > point.y ){
							y_min = point.y;
						}
					}
				}
				countSteps += slideLength;
                break;
            case 'W':
                for(int j = 0; j<slideLength; j++){
                    point.x -= 1;
					if( robotContainsThisPoint(point, robot.points, countSteps) ){
					//cout << point.x << "_____" << point.y << "\n";
						robot.points[index] = point;
						//cout << robot.points[index].x << "====" << robot.points[index].y << "\n";
						index++;
						totalFieldsCovered++;
						if(x_min > point.x ){
							x_min = point.x;
						}
					}
				}
				countSteps += slideLength;
                break;
            case 'E':
                for(int j = 0; j<slideLength; j++){
                    point.x += 1;
					if( robotContainsThisPoint(point, robot.points, countSteps) ){
					//cout << point.x << "_____" << point.y << "\n";
						robot.points[index] = point;
						//cout << robot.points[index].x << "====" << robot.points[index].y << "\n";
						index++;
						totalFieldsCovered++;
						if(x_max < point.x ){
							x_max = point.x;
						}
					}
				}
				countSteps += slideLength;
                break;
            default:
                cerr << "UNKNOWN DIRECTION";
                exit(EXIT_FAILURE);
        }
    }
	
	// converting max and min robot coords to size of a warehouse
	x_max = (width - 1) - x_max;
	x_min = 0 - x_min;
	y_max = (height - 1) - y_max;
	y_min = 0 - y_min;
	
	robot.pathLength = countSteps;
	robot.totalFieldsCovered = totalFieldsCovered;
	if (robot.totalFieldsCovered > coverageMax) {
		idMax = robot.id;
		coverageMax = robot.totalFieldsCovered;
	}
	robot.idIndex = 0;
	robot.merged = 0;	
	robot.ids = (int*) malloc( numberOfRobots * sizeof(int) );
	robot.ids[ robot.idIndex++ ] = robot.id;
	robot.x_min = x_min;
	robot.y_min = y_min;
	robot.x_max = x_max;
	robot.y_max = y_max;
	robot.possibilities = (robot.x_max - robot.x_min + 1) * (robot.y_max - robot.y_min + 1);
	return robot;
}

void printRobot(Robot robot) {
	cout << "Id: " << robot.id << "\n";
	cout << robot.pathLength;
	cout << " ";
	//cout << robot.numberOfSlides;
	//cout << " ";
	cout << robot.totalFieldsCovered;
	cout << "\n";
	cout << "x_max, x_min, y_max, y_min: " << robot.x_max << " " << robot.x_min << " " << robot.y_max << " " << robot.y_min << "\n";
}

void QuickSort(Robot *T, int Lo, int Hi){
   int i,j;
   int x;
   x = T[(Lo+Hi)>>1].totalFieldsCovered;
   i = Lo;
   j = Hi;
   do
   {
      while (T[i].totalFieldsCovered < x) ++i;
      while (T[j].totalFieldsCovered > x) --j;
      if (i<=j)
      {
         Robot tmp = T[i];
         T[i] = T[j];
         T[j] = tmp;
         ++i; --j;
      }
   } while(i < j);
   if (Lo < j) QuickSort(T, Lo, j);
   if (Hi > i) QuickSort(T, i, Hi);
}

//returns 1 if coords do not collide
int checkCollisions(Point * points, Point * points2, int length1, int length2){

	int k = 0;
	int m = 0;

	for(m = 0 ; m < length1; m++){
		for(k = 0 ; k < length2; k++){
			cout << points[m].x << " ooooooooooooooooooo " << points2[k].x << "\n";
			cout << points[m].y << " uuuuuuuuuuuuuuuuuuu " << points[k].y << "\n";
			if(points[m].x == points2[k].x && points[m].y == points2[k].y){
				cout << "HEJ\n";
				return 0;
				
			}
		}	
	}

	return 1;
}

void mergeVectors(Point * mergedVector, Point * vector1, Point * vector2, int l1 , int l2){
	int i = 0;
	int k = 0;

	for(i = 0; i < l1; i++)
		mergedVector[ k++ ] = vector1[i];

	for(i = 0; i < l2; i++)
		mergedVector[ k++ ] = vector2[i];

}

struct Robot mergeRobots(Robot robot1,Robot robot2){
	Robot mergedRobot;
	int i = 0;
	int j = 0;
									
	mergedRobot.idIndex = 0;
	mergedRobot.totalFieldsCovered = -1;

	mergedRobot.combinations =  (Point **) malloc ( robot1.possibilities * robot2.possibilities * sizeof (Point *) );
	mergedRobot.possibilities = robot1.possibilities * robot2.possibilities;
	
	for(i = 0; i < robot1.possibilities; i++){
		for(j = 0; j < robot2.possibilities; j++){
			cout << i << ":::" << j << "\n";
			if ( checkCollisions(robot1.combinations[i], robot2.combinations[j], robot1.totalFieldsCovered, robot2.totalFieldsCovered) == 1){
				cout << robot1.id << ":" << robot1.possibilities << " merge " << i << " " << j << " " << robot2.id << ":" << robot2.possibilities<< " \n";
				mergedRobot.combinations[ mergedRobot.merged++ ] = (Point*) malloc( (robot1.totalFieldsCovered + robot2.totalFieldsCovered ) * sizeof (Point));
				mergeVectors(mergedRobot.combinations[ mergedRobot.merged - 1 ], robot1.combinations[i], robot2.combinations[j], robot1.totalFieldsCovered, robot2.totalFieldsCovered);
				mergedRobot.totalFieldsCovered = (robot1.totalFieldsCovered + robot2.totalFieldsCovered );
				mergedRobot.ids[ mergedRobot.idIndex++ ] = robot2.id;
			}
		}
	}

	mergedRobot.combinations = (Point **) realloc (mergedRobot.combinations , mergedRobot.merged * sizeof (Point *) );

	return mergedRobot;
}

Point *  countCombinations(int x, int y, Point * points, int fieldsCovered){
	int i = 0;
	struct Point * newPoints = (Point *) malloc(fieldsCovered * sizeof * newPoints);
	memcpy(newPoints, points, fieldsCovered * sizeof(Point));

	for(i = 0; i < fieldsCovered; i++){
		newPoints[i].x += x;
		newPoints[i].y += y;
	}

	return newPoints;
}

struct Robot prepareCombinations(Robot robot){
	int i = 0;
	double **table;

	int x = 0;
	int y = 0;
	int v = 0;

	robot.combinations =  (Point **) malloc (robot.possibilities * sizeof (Point *));
	for (i = 0; i < robot.possibilities; i++){
		robot.combinations[i] = (Point*) malloc(robot.totalFieldsCovered * sizeof (Point));
	}

	for(x = robot.x_min; x <= robot.x_max; x++){
		for(y = robot.y_min; y <= robot.y_max; y++){
			robot.combinations[v++] = countCombinations(x , y, robot.points, robot.totalFieldsCovered);
		}
	}

	return robot;
}

void printSolutionForOne(Robot * robots, int m){
	robots[m] = prepareCombinations(robots[m]);
	cout << 1 << "\n" << robots[m].id << " " << robots[m].combinations[0][0].x + 1 << " " << robots[m].combinations[0][0].y + 1;
	exit(EXIT_SUCCESS);
}

void printSolution(Robot * mergedTable, int m, Robot * unsortedRobots){
	int k = 0;
	int j = 0;
	while(k < mergedTable[m].idIndex ){
		cout << mergedTable[m].ids[k] << " " << mergedTable[m].combinations[0][j].y + 1 << " " << mergedTable[m].combinations[0][j].x + 1 << "\n";
		j += unsortedRobots[ mergedTable[m].ids[ k++ ] - 1 ].totalFieldsCovered;
	}
	exit(EXIT_SUCCESS);
}

	
	void printRobotCombinations(Robot robot) {
		cout << robot.id << ": \n";
		for (int k = 0; k<robot.possibilities; k++) {
			for (int l = 0; l<robot.totalFieldsCovered; l++) {
				cout << "(" << robot.combinations[k][l].x << ", " << robot.combinations[k][l].y << ") ";
				cout << "\n";
			}
			cout << "\n";
		}
	}

int main(int argc, char *argv[]){
	if(argc != 2){
		printf("Usage: ./main <file>\n");
		exit(EXIT_FAILURE);
	} 
	
	int flagz = 1;
	int merged = 0;
	int m = 0;
	
	freopen(argv[1], "r", stdin);
	
	cin >> width >> height >> fieldsToCover >> numberOfRobots;
	
	struct Robot *robots = (Robot *) malloc(numberOfRobots * sizeof * robots);
	struct Robot *unsortedRobots = (Robot *) malloc(numberOfRobots * sizeof * unsortedRobots);
	
	for(int i = 0 ; i < numberOfRobots; i++){
		robots[i] = parseInputForRobot();
		unsortedRobots[i] = robots[i];
		//for (int l = 0; l<robots[i].totalFieldsCovered; l++) {
			//cout << robots[i].id << ": " << robots[i].points[l].x << "-" << robots[i].points[l].y << "\n";
		//}
		//printRobot(robots[i]);
	}
	
	QuickSort(robots, 0, numberOfRobots -1);
	
	if (robots[numberOfRobots - 1].totalFieldsCovered >= fieldsToCover) {
		printSolutionForOne(robots, numberOfRobots -1);
	}
	
	for(int i = 0; i <= numberOfRobots - 1 ; i++){
		robots[i] = prepareCombinations(robots[i]);
		printRobotCombinations(robots[i]);
	}
	
	Robot * mergedTable = ( Robot* ) malloc( (numberOfRobots * (numberOfRobots -1) ) * sizeof (Robot));
	for(int i = numberOfRobots - 1; i >= 0 ; i--){
		for(int j = numberOfRobots - 1; j >= 0 ; j--){
			if(i != j){
				cout << "hello " << robots[i].id << " " << robots[j].id << "\n";
				mergedTable[ merged++ ] = mergeRobots(robots[i], robots[j]);
				//printRobotCombinations(mergedTable[merged -1]);
				if(mergedTable[ merged - 1 ].totalFieldsCovered >= fieldsToCover){
					m = merged - 1;
					break;
				}
			}
		}
		if(mergedTable[m].totalFieldsCovered >= fieldsToCover){
			flagz = 0;
			printf("%d\n", mergedTable[m].idIndex);
			break;
		}
	}
	
	while(flagz){
		QuickSort(mergedTable, 0, merged - 1);
		for(m = merged -1 ; m >= 0; m--){
			for(int i = numberOfRobots - 1; i >= 0; i--){
				if( mergedTable[m].totalFieldsCovered > 0 && !containsId(robots[i].id, mergedTable[m].ids, mergedTable[m].idIndex) ){
					mergedTable[m] = mergeRobots(mergedTable[m], robots[i]);

					if(mergedTable[m].totalFieldsCovered >= fieldsToCover) break;
				}
				if(mergedTable[m].totalFieldsCovered == -1){
					m = - 1;
					break;
				}
				
			}

			if(mergedTable[m].totalFieldsCovered >= fieldsToCover) break;
			
		}
		if(mergedTable[m].totalFieldsCovered >= fieldsToCover) break;
	}


	if(flagz) printf("%d\n", mergedTable[m].idIndex);
	printSolution(mergedTable, m, unsortedRobots);
	
	return 0;
}