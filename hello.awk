#! /usr/bin/awk -f
BEGIN {

# Print the squares from 1 to 10 the first way

	i=1;
	while (i <= 10) {
		printf "%s %u %s %u", "The square of ", i, " is ", i*i;
		printf "\n";
		i = i+1;
	}

# do it again, using more concise code

	for (i=1; i <= 10; i++) {
		printf "%s %u %s %u","The square of ", i, " is ", i*i;
		printf "\n";
	}

# now end
exit;
}
