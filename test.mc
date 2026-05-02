int a;
int b;
int c;

a = 10;
b = 20;
c = 5;

if (a < b) {
    print(a);

    if (c < a) {
        print(c);
    } else {
        print(b);
    }

} else {
    print(b);
}
