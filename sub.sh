#! /bin/bash -x

number=10

sed -i 's/#Power Level:.*/#Power Level: '$number'/g' configuration1.txt
sed -i 's/PWL:.*/PWL:'$number'/g' configuration1.txt

number=$(($number+10))

sed -i 's/#Power Level:.*/#Power Level: '$number'/g' configuration1.txt
sed -i 's/PWL:.*/PWL:'$number'/g' configuration1.txt
