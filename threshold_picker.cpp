#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

#define MAX_BINARY_VALUE 255
#define MAX_GRAY_VALUE 255

#define WINDOW_TITLE "Threshold Picker"

#define EROSION 3
#define DILATION 4
#define OPENING 5
#define CLOSING 6

char *applied_effects = NULL ;

using namespace cv ;


//When using a single threshold set this to 255
int threshold_upper_value = 255 ; 
int threshold_lower_value = 0 ;

//Declaration of image handlers
Mat source, modified, gray, binary ;

//Prototype of trackbar envent handler function
void trackbar_event(int , void *) ;

//Prototype of effects menu function
int effects_menu() ;

//Prototype of apply threshold function
void apply_threshold() ;

//Prototype of write effects to string function
void write_effects_str(int operation, int kernel_type, int kernel_size) ;

//Prototype of function apply dilation
void apply_dilation() ;

int main(int argc, char *argv[]) {


	if (argc != 2) {
		printf("usage: %s <filename>\n", argv[0]) ;
		exit(2) ;
	}

	//Reads the original image and converts it to gray scale
	source = imread(argv[1], CV_LOAD_IMAGE_COLOR) ;
	modified = source.clone() ;
	cvtColor(source, gray, CV_BGR2GRAY) ;


	namedWindow(WINDOW_TITLE, CV_WINDOW_AUTOSIZE) ;

	//Upper threshold value trackbar
	createTrackbar("Threshold upper value", WINDOW_TITLE, &threshold_upper_value, MAX_BINARY_VALUE, trackbar_event) ;
	createTrackbar("Threshold lower value", WINDOW_TITLE, &threshold_lower_value, MAX_BINARY_VALUE, trackbar_event) ;

	trackbar_event(0, 0) ;

	waitKey(1) ;
	//Keep showing menu until user select to exit
	while(effects_menu()) ;

	return 0 ;
}


void apply_threshold() {
	cvtColor(modified, gray, CV_BGR2GRAY) ;
	//Apply a dual side threshold on gray image to binarize it
	inRange(gray, threshold_lower_value, threshold_upper_value, binary) ;
}

void trackbar_event(int , void *) {
	apply_threshold() ;
	imshow(WINDOW_TITLE, binary) ;
	waitKey(1) ;
}

void clear_effects() {
	//Deallocate the modified image and makes a copy of the original
	modified.deallocate() ;
	modified = source.clone() ;
	//Also clears the applied effects string
	free(applied_effects) ;
	applied_effects = NULL  ;
}

void read_kernel(int *type, int *size) {
	printf("Which type of kernel do you want?\n"
			"0 - Rectangular\n"
			"1 - Cross\n"
			"2 - Elliptcal\n") ;
	scanf("%d", type) ;

	printf("Enter the size of the kernel: ") ;
	scanf("%d", size) ;
}


void write_effects_str(int operation, int kernel_type, int kernel_size) {
	char buffer[1024] = "" ;

	switch (operation) {
		case EROSION:
			strcat(buffer, "Erosion: ") ;
			break ;
		case DILATION:
			strcat(buffer, "Dilation: ") ;
			break ;
		case OPENING:
			strcat(buffer, "Opening: ") ;
			break ;
		case CLOSING:
			strcat(buffer, "Closing: ") ;
			break ;
	}

	char kernel_type_str[20] ;
	switch (kernel_type) {
		case 0:
			strcpy(kernel_type_str, "Rectangular") ;
			break ;
		case 1:
			strcpy(kernel_type_str, "Cross") ;
			break ;
		case 2:
			strcpy(kernel_type_str, "Elliptical") ;
			break ;
	}	
	sprintf(buffer, "%sKernel(%s, %d)\n", buffer, kernel_type_str, kernel_size) ;

	int len_appl_eff = 0 ;
	if (applied_effects != NULL) {
		len_appl_eff = strlen(applied_effects) ;
	}

	if (applied_effects == NULL) {
		applied_effects = (char *) malloc(sizeof(char)) ;
		applied_effects[0] = '\0' ;
	}
	applied_effects = (char *) realloc(applied_effects, len_appl_eff+strlen(buffer)+1) ;
	strcat(applied_effects, buffer) ;
}

void apply_erosion() {
	int type, size ;
	read_kernel(&type, &size) ;

	Mat kernel = getStructuringElement(type, Size(2*size+1, 2*size+1), Point(size, size)) ;
	erode(modified, modified, kernel) ;

	write_effects_str(EROSION, type, size) ;	
}

void apply_dilation() {
	int type, size ;
	read_kernel(&type, &size) ;

	Mat kernel = getStructuringElement(type, Size(2*size+1, 2*size+1), Point(size, size)) ;
	dilate(modified, modified, kernel) ;

	write_effects_str(DILATION, type, size) ;	
}

void apply_opening() {
	int type, size ;
	read_kernel(&type, &size) ;

	Mat kernel = getStructuringElement(type, Size(2*size+1, 2*size+1), Point(size, size)) ;
	morphologyEx(modified, modified, MORPH_OPEN, kernel) ;

	write_effects_str(OPENING, type, size) ;	
}


void apply_closing() {
	int type, size ;
	read_kernel(&type, &size) ;

	Mat kernel = getStructuringElement(type, Size(2*size+1, 2*size+1), Point(size, size)) ;
	morphologyEx(modified, modified, MORPH_CLOSE, kernel) ;

	write_effects_str(CLOSING, type, size) ;	
}

int effects_menu() {
	printf("Effect menu\n\n"
			"Choose an option:\n"
			"0 - Exit\n"
			"1 - Clear effects\n"
			"2 - Show current effects\n"
			"3 - Apply erosion\n"
			"4 - Apply dilation\n"
			"5 - Apply closing\n"
			"6 - Apply opening\n"
			"7 - Change threshold values\n\n"
			) ;

	int option = 0 ;
	scanf("%d", &option) ;

	switch (option) {
		case 1:
			clear_effects() ;
			break ;
		case 2:
			if (applied_effects == NULL) {
				printf("No effects applied!") ;
			} else {
				printf(applied_effects) ;
			}
			printf("\n") ;
			break ;
		case 3:
			apply_erosion() ;
			break ;
		case 4:
			apply_dilation() ;
			break ;
		case 5:
			apply_opening() ;
			break ;
		case 6:
			apply_closing() ;
			break ;
		case 7:
			printf("Use the trackbas to change the threshold values, then press any key on window.\n") ;
			waitKey(0) ;
			break ;
	}

	trackbar_event(0,0) ;

	return option ;
}
