"""import sys
import base64

if len(sys.argv) != 2:
    print("Usage: python encode_base64.py <string>")
    sys.exit(1)

input_string = sys.argv[1]
encoded_string = base64.b64encode(input_string.encode()).decode()
print(encoded_string)"""

#!/usr/bin/env python3

import cgi
import cgitb
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

cgitb.enable()  # Enable CGI error reporting

form = cgi.FieldStorage()
input_string = form.getvalue("input_string")
print(f"value: {input_string}")
if input_string:
    encoded_string = base64.b64encode(input_string.encode()).decode()
    print(f"""
    {style}
    <html>
    <body>
        <h2>Base64 Encoder</h2>
        <p>Original String: {input_string}</p>
        <p>Base64 Encoded: {encoded_string}</p>
        <a href="/cgi.html">Encode another string</a>
    </body>
    </html>
    """)
else:
    print(f"""
    {style}
    <html>
    <body>
        <h2>Base64 Encoder</h2>
        <p>Error: No input provided.</p>
        <a href="/cgi.html">Try again</a>
    </body>
    </html>
    """)
