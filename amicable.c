#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

void show_progress(int current, int total, time_t start_time) {
    const int width = 35;

    float ratio = (float)current / total;
    int filled = (int)(ratio * width);

    // --- ETA calculation ---
    time_t now = time(NULL);
    double elapsed = difftime(now, start_time);

    double eta = 0.0;
    if (ratio > 0.0)
        eta = elapsed * (1.0 / ratio - 1.0);
    
    int eta_min = (int)(eta / 60);
    int eta_sec = (int)eta % 60;

    // --- Build full line in buffer (no flicker) ---
    char line[512];
    int pos = 0;

    pos += sprintf(line + pos, "\033[?7l\r[");  // disable wrap + CR

    for (int i = 0; i < filled; i++)
        pos += sprintf(line + pos, "█");

    for (int i = filled; i < width; i++)
        pos += sprintf(line + pos, "░");

    pos += sprintf(line + pos, "] %3d%%  ETA %02d:%02d",
                   (int)(ratio * 100), eta_min, eta_sec);

    fwrite(line, 1, pos, stdout);
    fflush(stdout);
}

int integer_sqrt(int n) {
    if (n < 0) return 0;
    if (n == 0 || n == 1) return n;

    int root = (int)(sqrt((double)n) + 0.5);

    while ((root + 1) * (root + 1) <= n) root++;
    while (root * root > n) root--;

    return root;
}

int calcSum(int num) {
    int sum = 1;
    int sqrtNum = integer_sqrt(num);
    for (int i = 2; i <= sqrtNum; i++) {
        if (num % i == 0) {
            sum += i + (num / i);
        }
    }
    return sum;
}

int main(int argc, char *argv[]) {
    int number;

    if (argc > 1) {
        // Try to convert the first argument to an integer
        number = atoi(argv[1]);
        printf("Using provided argument: %d\n", number);
    } else {
        // No argument provided → ask the user
 
        printf("Enter an integer between 100 and 1'000'000'000: ");

        while (scanf("%d", &number) != 1 || number < 100 || number > 1000000000) {
            while (getchar() != '\n');
            printf("Invalid input! Please enter an integer between 100 and 1'000'000'000: ");
        }
    }

    printf("Scanning up to %d...\n", number);

    // store results dynamically
    int capacity = 16;
    int count = 0;
    int (*pairs)[2] = malloc(capacity * sizeof *pairs);

    time_t start_time = time(NULL);
    for (int i = 1; i < number; i++) {

        // update progress bar
//        show_progress(i, number);
	if (i % 500 == 0)   // update only every 500 iterations
    	    // show_progress(i, number);
	    show_progress(i, number, start_time);

        int sum = calcSum(i);
        if (sum > i) {
            int sumsum = calcSum(sum);
            if (sumsum == i) {

                if (count == capacity) {
                    capacity *= 2;
                    pairs = realloc(pairs, capacity * sizeof *pairs);
                }

                pairs[count][0] = i;
                pairs[count][1] = sum;
                count++;
            }
        }
    }
    show_progress(number, number, start_time);

    printf("\033[?7h"); // re-enable line wrap
    printf("\n\nDone.\n\n");

    if (count == 0) {
        printf("No amicable pairs found.\n");
    } else {
	printf("========= Result ==========\n");
        printf("Found %d amicable pairs:\n", count);
        for (int i = 0; i < count; i++) {
            printf("(%d)\t%d / %d\n", i + 1, pairs[i][0], pairs[i][1]);
        }
	printf("===========================\n");
    }

    free(pairs);
    return 0;
}
