function iterate() {
	for i = 10; i != 0; i-- {
		print i;
	}

	if (i == 0) {
		return 1;
	}
	else {
		return 0;
	}

	return "test";
}

print iterate();
print "ok";

print (1 + 2) * 3;
