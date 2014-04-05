function double(a) {
	return (a * 2);
}

function test() {
	return 1 == "1";
}

print test();

if (test()) {
	print "Works!";
}
else {
	print "Does not work!";
}