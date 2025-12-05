#!/usr/bin/env python3
"""
Advent of Code client script for automatic login and puzzle interaction.
Uses session cookie authentication.
"""

import os
import sys
import requests
import re
from pathlib import Path
from typing import Optional

try:
    from bs4 import BeautifulSoup
    HAS_BS4 = True
except ImportError:
    HAS_BS4 = False

# Base URL for Advent of Code
AOC_URL = "https://adventofcode.com"
SESSION_FILE = Path.home() / ".aoc_session"


def get_session_cookie() -> Optional[str]:
    """Get session cookie from environment variable or file."""
    # Try environment variable first
    session = os.getenv("AOC_SESSION")
    if session:
        return session
    
    # Try reading from .session_cookie in current directory
    local_session_file = Path(".session_cookie")
    if local_session_file.exists():
        return local_session_file.read_text().strip()
    
    # Try reading from home directory
    if SESSION_FILE.exists():
        return SESSION_FILE.read_text().strip()
    
    return None


def save_session_cookie(session: str) -> None:
    """Save session cookie to file."""
    SESSION_FILE.write_text(session)
    SESSION_FILE.chmod(0o600)  # Make it readable only by owner
    print(f"Session cookie saved to {SESSION_FILE}")


def create_session() -> requests.Session:
    """Create a requests session with authentication."""
    session = requests.Session()
    cookie = get_session_cookie()
    
    if not cookie:
        print("Error: No session cookie found!")
        print("\nTo get your session cookie:")
        print("1. Log in to https://adventofcode.com in your browser")
        print("2. Open browser developer tools (F12)")
        print("3. Go to Application/Storage > Cookies")
        print("4. Copy the value of the 'session' cookie")
        print("5. Set it as an environment variable: export AOC_SESSION='your-cookie-value'")
        print(f"   Or save it to: {SESSION_FILE}")
        sys.exit(1)
    
    session.cookies.set("session", cookie, domain=".adventofcode.com")
    return session


def fetch_input(session: requests.Session, year: int, day: int) -> str:
    """Fetch puzzle input for a given year and day."""
    url = f"{AOC_URL}/{year}/day/{day}/input"
    response = session.get(url)
    
    if response.status_code == 200:
        return response.text.rstrip()
    elif response.status_code == 404:
        print(f"Error: Puzzle for {year}/day/{day} not found or not available yet")
        sys.exit(1)
    elif response.status_code == 400:
        print("Error: Bad request. Check your session cookie.")
        sys.exit(1)
    else:
        print(f"Error: Failed to fetch input (status {response.status_code})")
        sys.exit(1)


def submit_answer(session: requests.Session, year: int, day: int, part: int, answer: str) -> None:
    """Submit an answer for a puzzle."""
    url = f"{AOC_URL}/{year}/day/{day}/answer"
    data = {
        "level": str(part),
        "answer": str(answer)
    }
    
    response = session.post(url, data=data)
    
    if response.status_code != 200:
        print(f"Error: Failed to submit answer (status {response.status_code})")
        sys.exit(1)
    
    # Parse response to check if answer is correct
    text = response.text
    if "That's the right answer" in text or "That's correct" in text:
        print("✓ Correct answer!")
    elif "That's not the right answer" in text:
        print("✗ Wrong answer")
        if "too high" in text.lower():
            print("  (Hint: Your answer is too high)")
        elif "too low" in text.lower():
            print("  (Hint: Your answer is too low)")
    elif "You don't seem to be solving the right level" in text:
        print("Error: You've already completed this part")
    elif "You gave an answer too recently" in text:
        print("Error: You need to wait before submitting again")
        # Try to extract wait time
        wait_match = re.search(r"wait (\d+) (second|minute)", text)
        if wait_match:
            print(f"  Wait {wait_match.group(1)} {wait_match.group(2)}s")
    else:
        print("Unknown response. Check the website manually.")


def fetch_description(session: requests.Session, year: int, day: int) -> str:
    """Fetch puzzle description for a given year and day."""
    url = f"{AOC_URL}/{year}/day/{day}"
    response = session.get(url)
    
    if response.status_code == 200:
        return response.text
    elif response.status_code == 404:
        print(f"Error: Puzzle for {year}/day/{day} not found or not available yet")
        sys.exit(1)
    elif response.status_code == 400:
        print("Error: Bad request. Check your session cookie.")
        sys.exit(1)
    else:
        print(f"Error: Failed to fetch description (status {response.status_code})")
        sys.exit(1)


def extract_description_html(html: str) -> str:
    """Extract puzzle description from HTML page."""
    if HAS_BS4:
        soup = BeautifulSoup(html, 'html.parser')
        articles = soup.find_all('article', class_='day-desc')
        if articles:
            # Combine all article content
            description = '\n\n'.join(str(article) for article in articles)
            return description
        else:
            return ""
    else:
        # Fallback: use regex to extract article tags
        pattern = r'<article class="day-desc">(.*?)</article>'
        matches = re.findall(pattern, html, re.DOTALL)
        if matches:
            return '\n\n'.join(f'<article class="day-desc">{match}</article>' for match in matches)
        return ""


def extract_description_text(html: str) -> str:
    """Extract puzzle description as plain text (markdown-like)."""
    if HAS_BS4:
        soup = BeautifulSoup(html, 'html.parser')
        articles = soup.find_all('article', class_='day-desc')
        if articles:
            # Convert to text, preserving some structure
            descriptions = []
            for article in articles:
                # Get text and clean it up
                text = article.get_text(separator='\n', strip=True)
                descriptions.append(text)
            return '\n\n'.join(descriptions)
        return ""
    else:
        # Fallback: strip HTML tags
        html_content = extract_description_html(html)
        if html_content:
            # Basic HTML tag removal
            text = re.sub(r'<[^>]+>', '', html_content)
            text = re.sub(r'\n\s*\n', '\n\n', text)  # Normalize whitespace
            return text.strip()
        return ""


def download_input(year: int, day: int, output_file: Optional[str] = None) -> None:
    """Download puzzle input and save to file."""
    session = create_session()
    input_text = fetch_input(session, year, day)
    
    if output_file is None:
        output_file = f"{day}.txt"
    
    Path(output_file).write_text(input_text)
    print(f"Input saved to {output_file}")


def download_description(year: int, day: int, output_file: Optional[str] = None, format: str = "html") -> None:
    """Download puzzle description and save to file."""
    if format == "html" and not HAS_BS4:
        print("Warning: BeautifulSoup4 not installed. HTML extraction may be less accurate.")
        print("Install with: pip install beautifulsoup4")
    
    session = create_session()
    html = fetch_description(session, year, day)
    
    if format == "html":
        description = extract_description_html(html)
        if not description:
            print("Warning: Could not extract description from HTML")
            description = html  # Fallback to full page
    elif format == "text":
        description = extract_description_text(html)
        if not description:
            print("Warning: Could not extract description text")
            # Fallback: try to get at least something
            if HAS_BS4:
                soup = BeautifulSoup(html, 'html.parser')
                description = soup.get_text()
            else:
                description = re.sub(r'<[^>]+>', '', html)
    else:
        print(f"Error: Unknown format '{format}'. Use 'html' or 'text'")
        sys.exit(1)
    
    if output_file is None:
        ext = "html" if format == "html" else "md"
        output_file = f"{day}_description.{ext}"
    
    Path(output_file).write_text(description)
    print(f"Description saved to {output_file}")


def main():
    """Main entry point."""
    import argparse
    
    parser = argparse.ArgumentParser(description="Advent of Code client")
    parser.add_argument("--year", type=int, default=2025, help="Year (default: 2025)")
    parser.add_argument("--day", type=int, required=True, help="Day number (1-25)")
    parser.add_argument("--download", action="store_true", help="Download input")
    parser.add_argument("--description", action="store_true", help="Download puzzle description")
    parser.add_argument("--format", choices=["html", "text"], default="html", 
                       help="Format for description (html or text, default: html)")
    parser.add_argument("--output", type=str, help="Output file for download")
    parser.add_argument("--submit", type=int, choices=[1, 2], help="Submit answer (part 1 or 2)")
    parser.add_argument("--answer", type=str, help="Answer to submit")
    parser.add_argument("--save-session", type=str, help="Save session cookie from argument")
    
    args = parser.parse_args()
    
    # Handle saving session cookie
    if args.save_session:
        save_session_cookie(args.save_session)
        print("Session cookie saved!")
        return
    
    # Validate day
    if args.day < 1 or args.day > 25:
        print("Error: Day must be between 1 and 25")
        sys.exit(1)
    
    session = create_session()
    
    if args.download:
        download_input(args.year, args.day, args.output)
    elif args.description:
        download_description(args.year, args.day, args.output, args.format)
    elif args.submit:
        if not args.answer:
            print("Error: --answer required when submitting")
            sys.exit(1)
        submit_answer(session, args.year, args.day, args.submit, args.answer)
    else:
        # Default: just test connection
        print(f"Connected to Advent of Code {args.year}")
        print(f"Session cookie is valid: {session.cookies.get('session') is not None}")


if __name__ == "__main__":
    main()

