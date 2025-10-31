// C++ code
#include <math.h>


// This function sums up all values in an array
int mysum1(const int* numbers, int size) {
    int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += numbers[i];
    }
    return sum;
}


// This function calculates the parameterized nth Fibonacci number using an iterative approach
int myfib1(int n) {
    if (n <= 0) return 0;
    if (n == 1) return 1;
    int a = 0, b = 1, temp;
    for (int i = 2; i <= n; i++) {
        temp = a + b;
        a = b;
        b = temp;
    }
    return b;
}


// This function calculates and prints the x and y coordinates of points on a circle with radius r
// Radius (r) and number of points of the circle (numPoints) are parameterized for user
void plotcircle1(float r, int numPoints) {
  float x[numPoints];
  float y[numPoints];

  // Compute the x and y coordinates
  for (int i = 0; i < numPoints; i++) {
    // Calculate angle theta in radians
    float theta = (2 * PI * i) / numPoints;
    // Compute x and y using the parametric equation of a circle
    x[i] = r * cos(theta);
    y[i] = r * sin(theta);
  }

  // Print the x and y coordinates in the desired format
  // Print x cordinates
  Serial.print("x-coordinates: [");
  for (int i = 0; i < numPoints; i++) {
    Serial.print(x[i]);
    if (i < numPoints - 1) {
      Serial.print(", ");
    }
  }
  Serial.println("]");
  // Print y cordinates
  Serial.print("y-coordinates: [");
  for (int i = 0; i < numPoints; i++) {
    Serial.print(y[i]);
    if (i < numPoints - 1) {
      Serial.print(", ");
    }
  }
  Serial.println("]");
}


// This function generates and prints 50 random numbers between 1 and 100, with a delay for better visualization
void plotrand1() {
  for (int i = 0; i < 50; i++) {
    int randomNum = random(1, 101);  // Generate random number between 1 and 100
    Serial.print("Random number ");
    Serial.print(i + 1);  // Print the index of the random number (1-based)
    Serial.print(": ");
    Serial.println(randomNum);  // Print the random number
    delay(200);  // Add a small delay for better visualization
  }
}

void setup() {
    Serial.begin(9600);
    while (!Serial); // Wait for Serial Monitor
	// Test case for mysum1
    int test_cases[][5] = {
        {8, 0, 0, 0, 0},
        {-3, -7, -1, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {-4, 0, 0, 0, 0},
        {-1, 1, 2, -2, 3}
    };
  // Define the sizes of these arrays
    int sizes[] = {1, 3, 3, 0, 1, 5};
  // Print mysum1 sums of test case
    for (int i = 0; i < 6; i++) {
        Serial.print("Sum: ");
        Serial.println(mysum1(test_cases[i], sizes[i]));
    }


  // Test case for myfib1
    int fib_tests[] = {2, 8, 11, 15};
    for (int i = 0; i < 4; i++) {
        Serial.print("Fibonacci(");
        Serial.print(fib_tests[i]);
        Serial.print("): ");
        Serial.println(myfib1(fib_tests[i]));
    }

  // Call to plotcircle 1 with test case parameters
   plotcircle1(1, 4);
  // Call to plotrand1 to print 50 random integers as given in HW prompt
   plotrand1();

    while (true); // Halt execution to prevent looping
}

void loop() {
    // Empty loop to prevent re-execution
}
