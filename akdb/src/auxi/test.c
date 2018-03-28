/**
@file test.c Provides functions for reporting test results for modules.
 */


#include "test.h"

/**
 * @author Igor Rinkovec
 * @brief Returns the amount of successful and failed tests.
 * @param successfulAmount amount of successful tests
 * @param failedAmount amount of failed tests
 * @return TestResult
 */
TestResult TEST_result(int successfulAmount, int failedAmount) {
	TestResult result;
	result.testSucceded = successfulAmount;
	result.testFailed = failedAmount;
	result.implemented = true;
	return result;
}

/**
 * @author Igor Rinkovec
 * @brief Returns the TestResult object with an unimplemented flag so the maintainers are aware that the test still does not report its results correctly.
 * @return TestResult
 */
TestResult TEST_result() {
	TestResult result;
	result.implemented = false;
	return result;
}


/**
 * @author Igor Rinkovec
 * @brief Prints a beautiful string informing the user of test results in the terminal.
 * @return void
 */
void TEST_output_results(TestResult result) {
	if(result.implemented == false) {
		printf( YELLOW "TEST: Test is still not reporting the results correctly. Read https://github.com/mschatten/akdb/wiki/Reporting-Test-Results\n" RESET );
		return;
	}
	if(result.testSucceded == result.testFailed) {
		printf( GREEN "TEST: %i of %i tests successful.\n" RESET , result.testSucceded, result.testSucceded);
		return;
	}
	printf( RED "TEST: %i of %i tests FAILED.\n" RESET , result.testFailed, result.testSucceded);
}

