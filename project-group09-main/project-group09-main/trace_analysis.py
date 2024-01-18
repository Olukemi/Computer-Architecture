from ast import literal_eval
ran = False
prev = 0
current = 0
large = 0
max_stride = 64
counter = [0] * (max_stride + 1)

with open("g++.trace") as infile: #Modified to determine trace file anaylzed
    for line in infile:
        try:
            current = int(line[2:11],0) #Varied ending from 11 to 12 as needed
        except ValueError:
            current = current
        stride = current - prev #Check concurrent stride values
        print("Stride = " + str(stride)) #For debugging
        prev = current
        if(stride > 64):
            large += 1 #For values larger than 64, note as "large"
        else:
            for stride_value in range(max_stride + 1): #Comparison up to 64
                if stride == stride_value or (-1 * stride) == stride_value:
                    counter[stride_value] = counter[stride_value] + 1

print(counter)
print(large)
