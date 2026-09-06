import time
fin_ans = "y"
while fin_ans == "y":
    num1 = int(input("Enter first number: "))
    num2 = int(input("Enter second number: "))
    num3 = int(input("Enter third number: "))
    time.sleep(1)
    menu_op = "(+, -, *, /)"
    print(menu_op)
    op = input("Choose operator: ")
    if op == "+":
        ans = num1 + num2 + num3
        print(ans)
    elif op == "-":
        ans = num1 - num2 - num3
        print(ans)
    elif op == "*":
        ans = num1 * num2 * num3
        print(ans)
    elif op == "/":
        if num2 == 0 or num3 == 0:
            print("Can't divide by zero!")
        else:
            ans = num1 / num2 / num3
            print(ans)
    else:
        print("Invalid operator!")
    print("Done! Do you wanna redo?")
    fin_ans = input("y/n: ").lower()
print("Calculator closed.")
