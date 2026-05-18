#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>
#include <string.h>

// Formats a number with ' separators into a destination buffer
void format_num(long long val, char* dest) {
    char buf[32];
    int len = sprintf(buf, "%lld", val);
    int ticks = (len - 1) / 3;
    int res_len = len + ticks;

    dest[res_len] = '\0';
    int count = 0;
    for (int i = len - 1, j = res_len - 1; i >= 0; i--, j--) {
        dest[j] = buf[i];
        if (++count == 3 && i > 0 && buf[i-1] != '-') {
            dest[--j] = '\'';
            count = 0;
        }
    }
}

char* frt_num(int num){
    char* out1 = malloc(64 * sizeof(char));
    format_num(num, out1);
    return out1;
}

void show_progress(int current, int total, time_t start_time) {
    const int width = 32;

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

    pos += sprintf(line + pos, "] %3d%% ETA %02d:%02d",
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

    setlocale(LC_NUMERIC, "de_CH");

    if (argc > 1) {
        // Try to convert the first argument to an integer
        number = atoi(argv[1]);
        printf("Using provided argument: %'d\n", number);
    } else {
        // No argument provided → ask the user
 
        printf("Enter an integer between 100 and 1'000'000'000: ");

        while (scanf("%d", &number) != 1 || number < 100 || number > 1000000000) {
            while (getchar() != '\n');
            printf("Invalid input! Please enter an integer between 100 and 1'000'000'000: ");
        }
    }
    
    char* output;//[64];
    output = frt_num(number);

    printf("Scanning up to %s...\n", output);
    printf("\033[?25l");

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

    time_t now = time(NULL);
    double elapsed = difftime(now, start_time);
    int el_min = (int)(elapsed / 60);
    int el_sec = (int)elapsed % 60;

    printf("\033[?7h"); // re-enable line wrap
    printf("\n\nDone in: %02d:%02d\n\n", el_min, el_sec);

    if (count == 0) {
        printf("No amicable pairs found.\n");
    } else {
	printf("========= Result ==========\n");
	printf("From:\t1\n");
	printf("To:\t%s\n", output);
        printf("Found:\t%d amicable pairs\n", count);
	printf("‐--------------------------\n");
        for (int i = 0; i < count; i++) {
	    char* out1;//[64];
    	    out1 = frt_num(pairs[i][0]);
	    char* out2;//[64];
            out2 = frt_num(pairs[i][1]);
            printf("(%'d)\t%s / %s\n", i + 1, out1, out2);
	    free(out1);
	    free(out2);
        }
	printf("===========================\n");
    }
    //printf("teset: %s", frt_num(10000000));
    free(pairs);
    free(output);
    printf("\033[?25h");
    return 0;
}
