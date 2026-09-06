(lambda f: f(f))(lambda f:
    (lambda x:
        print(
            (lambda a,b,c:
                {"+" : a+b+c,
                 "-" : a-b-c,
                 "*" : a*b*c,
                 "/" : a/b/c}.get(
                    input("op: "),
                    "skill issue"
                )
            )(
                *map(int,[
                    input("num: "),
                    input("num: "),
                    input("num: ")
                ])
            )
        )
    )(None)
)
