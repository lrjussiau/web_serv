#!/usr/bin/python3
import sys
import base64

style = """<style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            margin-top: 50px;
        }
        button {
            padding: 10px 20px;
            font-size: 16px;
            cursor: pointer;
            background-color: #007BFF;
            color: white;
            border: none;
            border-radius: 5px;
        }
        button:hover {
            background-color: #0056b3;
        }
    </style>"""

# Check if the correct number of arguments is provided
if len(sys.argv) != 2:
    print(f"""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Base64 Encoding</title>
    {style}
    </head>
    <body>
        <h2>Base64 Encoder</h2>
        <p>Error: No input provided.</p>
        <a href="../encoder.html">Try again</a>
    </body>
    </html>
    """)
    sys.exit(1)

# Get the input string from command line argument
input_string = sys.argv[1]

# Check if the input is a string
if not isinstance(input_string, str):
    print(f"""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Base64 Encoding</title>
    {style}
    </head>
    <body>
        <h2>Base64 Encoder</h2>
        <p>Error: No input provided.</p>
        <a href="../encoder.html">Try again</a>
    </body>
    </html>
    """)
    sys.exit(1)
# Encode the input string to base64
encoded_string = base64.b64encode(input_string.encode()).decode()

# Print the HTML page with the encoded string
print("""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Base64 Encoding</title>
    {style}
</head>
<body>
    <h1>Base64 Encoded String:</h1>
    <p>{}</p>
    <button onclick="location.href='../index.html'">Go Back to Home</button>
</body>
</html>""".format(encoded_string, style=style))