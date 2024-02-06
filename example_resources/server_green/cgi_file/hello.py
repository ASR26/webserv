import sys

print("Hello from Python")
print("Arguments:")
if (len(sys.argv) > 1):
	i = 1
	while (i < len(sys.argv)):
		print(sys.argv[i])
		i += 1