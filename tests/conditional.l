// Output:
//
// Correct!
// Even though 'c' does not exist I should see this fine.
a = true;
b = false;

if (a or b) {
	print "Correct!";

	if (a and b) {
		print "Should not see this.";
	}

	if (a or c) {
		print "Even though 'c' does not exist I should see this fine.";
	}
}