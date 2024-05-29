#!/usr/bin/env python3
import os

UPLOAD_DIR = '../upload'

def list_files():
    files = os.listdir(UPLOAD_DIR)
    file_list_html = ""
    for file in files:
        file_list_html += f'<li><a href="/{UPLOAD_DIR}/{file}">{file}</a></li>'
    return file_list_html

def main():
    print("Content-Type: text/html\r\n\r\n")
    file_list_html = list_files()
    print(f"""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <title>File List</title>
    </head>
    <body>
        <ul>
            {file_list_html}
        </ul>
    </body>
    </html>
    """)

if __name__ == "__main__":
    main()
