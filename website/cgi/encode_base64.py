import sys
import base64

if len(sys.argv) != 2:
    print("Usage: python encode_base64.py <string>")
    sys.exit(1)

input_string = sys.argv[1]
encoded_string = base64.b64encode(input_string.encode()).decode()
print(encoded_string)
