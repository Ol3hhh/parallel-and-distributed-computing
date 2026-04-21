import math

n = 150
result = 0
for k in range(1, n+1):
    g = 1/k 
    result += g 
result -= math.log(n)
print(result)