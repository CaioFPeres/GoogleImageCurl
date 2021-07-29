#include "Main.h"

int main(int argc, char** argv) {

	char* query = (char*)malloc(5000 * sizeof(char));

	int amount = 0;

	if (argc > 3) {
		cout << "too many arguments";
		return 0;
	}
	else if (argc == 1) {

		cout << "query: ";
		cin.getline(query, 5000);
		cout << "pages (number): ";
		cin >> amount;

	}
	else if (argc <= 2) {
		cout << "too few arguments";
		return 0;
	}


	MainClass main = MainClass(amount, query);


	system("pause");

	return 0;

}