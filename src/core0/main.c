#define CPU0
#include "convolution.h"

#include "stdlib.h"
#include "stdio.h"

#include "platform.h"
#include "xil_printf.h"

#include "xscutimer.h"

/**
 * This function initializes the original
 * array with zeros.
 */
static void initialize_org_array()
{
    for (int i = 0; i < ARRAY_R_EX; i++) {
        for (int j = 0; j < ARRAY_C_EX; j++) {
            // put zero into the current index.
            ORIGINAL_ARRAY_INDEX(i,j) = (uint8_t) 0;
        }
    }
}

/**
 * This function fill the original array
 * with random values to create an 128x128
 * array. We ignore the perimeter and fill
 * only the 128x128 values of 130x130.
 */
static void create_128x128_array()
{
    srand(0);
    for (int i = 1; i <= ARRAY_R; i++) {
        for (int j = 1; j <= ARRAY_C; j++) {
            // Assign a random value into the current index.
            ORIGINAL_ARRAY_INDEX(i,j) = (uint8_t) (rand() % (255 - 0 + 1)) + 0;
        }
    }
}

static void calculate_array()
{
	int counter = 0;
    for (int i = 1; i <= ARRAY_R; i++) {
        for (int j = 1; j <= ARRAY_C; j++) {
        	PRODUCT_ARRAY_INDEX(i - 1, j - 1) = LEFT_TOP_PIXEL(i,j)       * (-1) +
                                                MIDDLE_TOP_PIXEL(i, j)    * (-1) +
                                                RIGHT_TOP_PIXEL(i, j)     * (-1) +
                                                LEFT_PIXEL(i, j)          * (-1) +
                                                MIDDLE_PIXEL(i, j) 		  * (9)  +
                                                RIGHT_PIXEL(i, j)         * (-1) +
                                                BOTTOM_LEFT_PIXEL(i, j)   * (-1) +
                                                BOTTOM_MIDDLE_PIXEL(i, j) * (-1) +
                                                BOTTOM_RIGHT_PIXEL(i, j)  * (-1);
            ++counter;
            // stop cpu0, at the center of the array.
            if (counter == (ARRAY_SIZE/2)) return;
        }
    }
}

/*static void verify(int i, int j)
{
	uint8_t e1 = LEFT_TOP_PIXEL(i,j);
	uint8_t e2 = MIDDLE_TOP_PIXEL(i, j);
	uint8_t e3 = RIGHT_TOP_PIXEL(i, j);
	uint8_t e4 = LEFT_PIXEL(i, j);
	uint8_t e5 = MIDDLE_PIXEL(i, j);
	uint8_t e6 = RIGHT_PIXEL(i, j);
	uint8_t e7 = BOTTOM_LEFT_PIXEL(i, j);
	uint8_t e8 = BOTTOM_MIDDLE_PIXEL(i, j);
	uint8_t e9 = BOTTOM_RIGHT_PIXEL(i, j);

	printf("Array to calculate: \n");
	printf("[ -1, -1, -1 ]  [%d,   %d,   %d   ]\n"
		   "[ -1,  9, -1 ]  [%d,   %d,   %d   ]\n"
		   "[ -1, -1, -1 ]  [%d,   %d,   %d   ]\n", e1, e2, e3, e4, e5, e6, e7, e8, e9);

	e1 = PRODUCT_ARRAY_INDEX(i - 1, j - 1);

	printf("Result [%d, %d]:\n", i - 1, j - 1);
	printf("%d\n", e1);
	printf("%p\n", &PRODUCT_ARRAY_INDEX(i - 1, j - 1));
}*/


int main(void)
{
    init_platform();
    Xil_SetTlbAttributes(0xFFFF0000, 0x14de2);

    if (STATUS_RESET == MUST_RESET ||
        STATUS_CPU0 != 0) {
        STATUS_CPU0 = 0;
        STATUS_CPU1 = 0;
        STATUS_RESET = 0;
	}

    initialize_org_array();
    // Make the original array.
    create_128x128_array();
    printf("[*] 128x128 array successfully created...\n");

    // start processing.
    STATUS_CPU0 = CPU0_DONE_GEN;

    // start calculation.
    calculate_array();

    while (STATUS_CPU1 != CPU1_DONE_PROC);
    printf("[*] 128x128 convolution matrix is done...\n");

    // temporal TODO - check it and delete it.
    /*printf("Verifying for the index [0,0]...\n");
    verify(1,1);
    printf("Verifying for the index [50,23]...\n");
    verify(51, 24);
    printf("Verifying for the index [128,128]...\n");
    verify(128,128);*/

    cleanup_platform();
}
