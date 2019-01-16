import copy
f = open("input.txt")
player = f.readline()
alg = f.readline()
dep = f.readline()
L = []
for i in range(8):
 s = f.readline()
 l = s.strip().split(",")
 L.append(l)
val = f.readline().strip().split(",")
nodes = 1
print player,alg,dep,L,val
def Result(L,pl):
	LL = []
	for i in range(len(L)):
		for j in range(len(L[i])):
			if L[i][j][0] == pl and pl =="S":
				if i > 0 and j > 0:
					L1 = copy.deepcopy(L)
					if i == 1 and L1[i-1][j-1][0] == pl:
						L1[i-1][j-1] = pl+str(int(L1[i-1][j-1][1])+1)
						if L1[i][j][1] == "1":
						    L1[i][j] = "0"
						else:
							L1[i][j] = pl+str(int(L1[i][j][1])-1)
						LL.append((L1,i,j,i-1,j-1))
					if L1[i-1][j-1][0] == "0":

						L1[i-1][j-1] = pl+"1"
						if L1[i][j][1] == "1":
						    L1[i][j] = "0"
						else:
							L1[i][j] = pl+str(int(L1[i][j][1])-1)
						LL.append((L1,i,j,i-1,j-1))
					if i > 1 and j > 1 and L1[i-1][j-1][0] == "C":

						if i == 2 and L1[i-2][j-2][0] == pl:
						    L1[i-2][j-2] = pl+str(int(L1[i-2][j-2][1])+1)    
						    if L1[i][j][1] == "1":
						        L1[i][j] = "0"
						    else:
							    L1[i][j] = pl+str(int(L1[i][j][1])-1)
						    L1[i-1][j-1] = "0"
						    LL.append((L1,i,j,i-2,j-2))
						elif L1[i-2][j-2][0] == "0":
						    L1[i-2][j-2] = pl+"1"
						    if L1[i][j][1] == "1":
						        L1[i][j] = "0"
						    else:
							    L1[i][j] = pl+str(int(L1[i][j][1])-1)
						    L1[i-1][j-1] = "0"
						    LL.append((L1,i,j,i-2,j-2))
                                if i > 0 and j < 7:

					L1 = copy.deepcopy(L)
					if i == 1 and L1[i-1][j+1][0] == pl:
						L1[i-1][j+1] = pl+str(int(L1[i-1][j+1][1])+1)
						if L1[i][j][1] == "1":
						    L1[i][j] = "0"
						else:
							L1[i][j] = pl+str(int(L1[i][j][1])-1)
						LL.append((L1,i,j,i-1,j+1))
					
					if L1[i-1][j+1][0] == "0":

						L1[i-1][j+1] = pl+"1"
						if L1[i][j][1] == "1":
						    L1[i][j] = "0"
						else:
							L1[i][j] = pl+str(int(L1[i][j][1])-1)
						LL.append((L1,i,j,i-1,j+1))
				
					if i > 1 and j < 6 and L1[i-1][j+1][0] == "C":

						if i == 2 and L1[i-2][j+2][0] == pl:
						    L1[i-2][j+2] = pl+str(int(L1[i-2][j+2][1])+1)    
						    if L1[i][j][1] == "1":
						        L1[i][j] = "0"
						    else:
							    L1[i][j] = pl+str(int(L1[i][j][1])-1)
						    L1[i-1][j+1] = "0"
						    LL.append((L1,i,j,i-2,j+2))
						
						elif L1[i-2][j+2][0] == "0":
						    L1[i-2][j+2] = pl+"1"
						    if L1[i][j][1] == "1":
						        L1[i][j] = "0"
						    else:
							    L1[i][j] = pl+str(int(L1[i][j][1])-1)
						    L1[i-1][j+1] = "0"
						    LL.append((L1,i,j,i-2,j+2))
		        if L[i][j][0] == pl and pl =="C":
				if i < 7 and j > 0:
					L1 = copy.deepcopy(L)
					if i == 6 and L1[i+1][j-1][0] == pl:
						L1[i+1][j-1] = pl+str(int(L1[i+1][j-1][1])+1)
						if L1[i][j][1] == "1":
						    L1[i][j] = "0"
						else:
							L1[i][j] = pl+str(int(L1[i][j][1])-1)
						LL.append((L1,i,j,i+1,j-1))
						
					if L1[i+1][j-1][0] == "0":

						L1[i+1][j-1] = pl+"1"
						if L1[i][j][1] == "1":
						    L1[i][j] = "0"
						else:
							L1[i][j] = pl+str(int(L1[i][j][1])-1)
						LL.append((L1,i,j,i+1,j-1))

					if i < 6  and j > 1 and L1[i+1][j-1][0] == "S":

						if i == 5 and L1[i+2][j-2][0] == pl:
						    L1[i+2][j-2] = pl+str(int(L1[i+2][j-2][1])+1)    
						    if L1[i][j][1] == "1":
						        L1[i][j] = "0"
						    else:
							    L1[i][j] = pl+str(int(L1[i][j][1])-1)
						    L1[i+1][j-1] = "0"
						    LL.append((L1,i,j,i+2,j-2))
						elif L1[i+2][j-2][0] == "0":
						    L1[i+2][j-2] = pl+"1"
						    if L1[i][j][1] == "1":
						        L1[i][j] = "0"
						    else:
							    L1[i][j] = pl+str(int(L1[i][j][1])-1)
						    L1[i+1][j-1] = "0"
						    LL.append((L1,i,j,i+2,j-2))
						    
				if i < 7 and j < 7:
					L1 = copy.deepcopy(L)
					if i == 6 and L1[i+1][j+1][0] == pl:
						L1[i+1][j+1] = pl+str(int(L1[i+1][j+1][1])+1)
						if L1[i][j][1] == "1":
						    L1[i][j] = "0"
						else:
							L1[i][j] = pl+str(int(L1[i][j][1])-1)
						LL.append((L1,i,j,i+1,j+1))
						
					if L1[i+1][j+1][0] == "0":

						L1[i+1][j+1] = pl+"1"
						if L1[i][j][1] == "1":
						    L1[i][j] = "0"
						else:
							L1[i][j] = pl+str(int(L1[i][j][1])-1)
						LL.append((L1,i,j,i+1,j+1))

					if i < 6 and j < 6 and L1[i+1][j+1][0] == "S":

						if i == 5 and L1[i+2][j+2][0] == pl:
						    L1[i+2][j+2] = pl+str(int(L1[i+2][j+2][1])+1)    
						    if L1[i][j][1] == "1":
						        L1[i][j] = "0"
						    else:
							    L1[i][j] = pl+str(int(L1[i][j][1])-1)
						    L1[i+1][j+1] = "0"
						    LL.append((L1,i,j,i+2,j+2))
						elif L1[i+2][j+2][0] == "0":
						    L1[i+2][j+2] = pl+"1"
						    if L1[i][j][1] == "1":
						        L1[i][j] = "0"
						    else:
							    L1[i][j] = pl+str(int(L1[i][j][1])-1)
						    L1[i+1][j+1] = "0"
						    LL.append((L1,i,j,i+2,j+2))
	return LL

def terminal(L,dep):
	global nodes
	if dep == 0:
		return True
	s = 0
	c = 0
	for i in range(len(L)):
		for j in range(len(L[i])):
			if L[i][j] != "0" and L[i][j][0] == "S":
				s += 1
			if L[i][j] != "0" and L[i][j][0] == "C":
				c += 1
	if s == 0 or c == 0:
		return True
	if Result(L,"S") == [] and Result(L,"C") == []:
		nodes+=2
		return True
	return False
def Utility(L,val):
	sum_s = 0
	sum_c = 0
	for i in range(len(L)):
		for j in range(len(L[i])):
			if L[i][j]!="0":
				if L[i][j][0] == "S":
					sum_s += int(val[len(L) - i - 1])*int(L[i][j][1])
				if L[i][j][0] == "C":
					sum_c += int(val[i])*int(L[i][j][1])
	return sum_s - sum_c


def MaxVal(L,player,dep,val):
	global nodes
	pl = player[0]
	if pl =="S":
		op = "C"
	else:
		op = "S"
	if terminal(L,dep):
		if pl =="S":
			return Utility(L,val)
		else:
			return -Utility(L,val)
	v = float('-inf')
	LL = Result(L,pl)
	if not LL:
		nodes+=1
		v = MinVal(L,player,dep-1,val)
	for L in LL:
		nodes+=1
		v = max(v,MinVal(L[0],player,dep-1,val))
	return v
def MinVal(L,player,dep,val):
	global nodes
	pl = player[0]
	if pl =="S":
		op = "C"
	else:
		op = "S"
	if terminal(L,dep):
		if pl =="S":
			return Utility(L,val)
		else:
			return -Utility(L,val)
	v = float('inf')
	LL = Result(L,op)
	if not LL:
		nodes+=1
		v = MaxVal(L,player,dep-1,val)
	
	for L in LL:
		nodes+=1
		v = min(v,MaxVal(L[0],player,dep-1,val))
	return v
def Minimax(L,player,dep,val):
	global nodes
	dep = int(dep)
	pl = player[0]
	if terminal(L,dep):
		if pl =="S":
			return Utility(L,val)
		else:
			return -Utility(L,val)
	v = float('-inf')
	LL = Result(L,pl)
	if not LL:
		nodes+=1
		v = MinVal(L,player,dep-1,val)
		step = "pass"
		nextval = Utility(L,val)
	else:
		for L in LL:
			nodes+=1
			vv = max(v,MinVal(L[0],player,dep-1,val))
			if vv != v:
				Llast = L
				v = vv
		ss = "HGFEDCBA"
		step = ss[Llast[1]]+str(Llast[2]+1)+"-"+ss[Llast[3]]+str(Llast[4]+1)
		if pl =="S":
			nextval =  Utility(Llast[0],val)
		else:
			nextval =  -Utility(Llast[0],val)
	filestring = step+"\n"+str(nextval)+"\n"+str(v)+"\n"+str(nodes)
	f1 = open("output.txt","w")
	f1.write(filestring)
	print  step,"\n",nextval,"\n",v,"\n",nodes
	return v


def ABMaxVal(L,player,dep,val,a,b):
	global nodes
	pl = player[0]
	if pl =="S":
		op = "C"
	else:
		op = "S"
	if terminal(L,dep):
		if pl =="S":
			return (Utility(L,val),(L,None,None,None,None))
		else:
			return (-Utility(L,val),(L,None,None,None,None))
	v = float('-inf')
	LL = Result(L,pl)
	if not LL:
		nodes+=1
		v = ABMinVal(L,player,dep-1,val,a,b)[0]
		Llast = (L,None,None,None,None)
	for L in LL:
		nodes+=1
		vv = max(v,ABMinVal(L[0],player,dep-1,val,a,b)[0])
		if vv != v:
				Llast = L
				v = vv
		if v >= b:
			return (v,Llast)
		a = max(a,v)
	return (v,Llast)
def ABMinVal(L,player,dep,val,a,b):
	global nodes
	pl = player[0]
	if pl =="S":
		op = "C"
	else:
		op = "S"
	if terminal(L,dep):
		if pl =="S":
			return (Utility(L,val),(L,None,None,None,None))
		else:
			return (-Utility(L,val),(L,None,None,None,None))
	v = float('inf')
	LL = Result(L,op)
	if not LL:
		nodes+=1
		v = ABMaxVal(L,player,dep-1,val,a,b)[0]
		Llast = (L,None,None,None,None)
	
	for L in LL:
		nodes+=1
		vv = min(v,ABMaxVal(L[0],player,dep-1,val,a,b)[0])
		if vv != v:
				Llast = L
				v = vv
		if v <= a:
			return (v,Llast)
		b = min(b,v)
	return (v,Llast)

def ABsearch(L,player,dep,val):
	global nodes
	dep = int(dep)
	pl = player[0]
	a = float("-inf")
	b = float("inf")
	Res = ABMaxVal(L,player,dep,val, a,b)
	v = Res[0]
	Llast = Res[1]
	if not Llast[1]:
		step = "pass"
	else:
		ss = "HGFEDCBA"
		step = ss[Llast[1]]+str(Llast[2]+1)+"-"+ss[Llast[3]]+str(Llast[4]+1)
	if pl =="S":
		nextval =  Utility(Llast[0],val)
	else:
		nextval =  -Utility(Llast[0],val)
	filestring = step+"\n"+str(nextval)+"\n"+str(v)+"\n"+str(nodes)
	f1 = open("output.txt","w")
	f1.write(filestring)
	print  step,"\n",nextval,"\n",v,"\n",nodes
def search(L,player,dep,val):
	if alg.strip() == "ALPHABETA":
		ABsearch(L,player,dep,val)
	else:
		Minimax(L,player,dep,val)
search(L,player,dep,val)



