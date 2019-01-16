import copy
f = open("input.txt")
GroCount = f.readline()
PotCount = f.readline()
pots = []
for i in range(PotCount):
	pots[i] = f.readline()
confederation = []
for i in range(6):
	confederation[i] = f.readline()
print GroCount
print PotCount
print pots
print confederation
