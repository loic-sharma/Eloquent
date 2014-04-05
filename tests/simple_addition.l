function double(a) {
	return 2 * a;
}

function four() {
	return double(double(1));
}

print four();