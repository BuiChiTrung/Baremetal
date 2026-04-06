import sys
import time
import random

def main():
    if len(sys.argv) != 2:
        print("Usage: python random_gen.py <seconds>")
        sys.exit(1)

    try:
        delay = float(sys.argv[1])
    except ValueError:
        print("Error: Delay must be a number.")
        sys.exit(1)

    print(f"Generating random numbers (0-12) every {delay}s. Press Ctrl+C to stop.")
    
    try:
        while True:
            # random.randint(a, b) includes both endpoints: 0 and 12.
            print(random.randint(0, 12))
            time.sleep(delay)
    except KeyboardInterrupt:
        print("\nStopped.")

if __name__ == "__main__":
    main()
