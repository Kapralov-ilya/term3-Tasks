#include <iostream>
#include <cstring>

using namespace std;

class Array_string{
        char **array;
        int available;
        int used;
public:
        Array_string();
        Array_string(const Array_string& first_array);
        ~Array_string();
        void print_array() const;
        void print_string(int i) const;
        void add_string(const char *string);
        char *get_string(int num) const {return num<used ? array[num] : 0;}
        static char *duplicate_string(const char *string);
        static int length_string(const char *string);
        bool check_equal(const char *string) const;
        const Array_string& operator+=(const Array_string& first_array);
        friend Array_string operator+(const Array_string& first_array,const Array_string& second_array);
};

Array_string::Array_string(){
        array=new char*[16];
        available=16;
        used=0;
}

Array_string::Array_string(const Array_string& first_array){
        array=0;
        available=0;
        used=0;
        for(int i=0;i<first_array.used;i++) add_string(first_array.get_string(i));
}

Array_string::~Array_string(){
        for(int i=0;i<used;i++) delete []array[i];
        delete []array;
}

void Array_string::print_array() const{
        for(int i=0;i<used;i++) cout<<array[i]<<endl;
}

void Array_string::print_string(int i) const{
        if (i<used) cout<<array[i]<<endl;
}

void Array_string::add_string(const char *string){
        char **tempary;
        if (used==available){
                available+=16;
                tempary=new char*[available];
                for(int i=0;i<used;i++){
                         tempary[i]=array[i];
                }
                delete []array;
                array=tempary;
        }
        array[used++]=duplicate_string(string);
}

int Array_string::length_string(const char *string){
        int i;
        for(i=0;string[i];i++);
        return i;
}

char *Array_string::duplicate_string(const char *string){
        int length=length_string(string);
        char *copy=new char[length+1];
        for(int i=0;string[i];i++) copy[i]=string[i];
        copy[length]=0;
        return copy;
}

bool Array_string::check_equal(const char *string) const{
        for (int i=0;i<used;i++){
                if (!strcmp(get_string(i),string)) return false;
        }
        return true;
}

Array_string operator+(const Array_string& first_array,const Array_string& second_array){
        Array_string result;
        for(int i=0;i<first_array.used;i++){
                result.add_string(first_array.get_string(i));
        }
        for(int i=first_array.used;i<(first_array.used+second_array.used);i++){
                result.add_string(second_array.get_string(i-first_array.used));
        }
        return result;
}

const Array_string& Array_string::operator+=(const Array_string& first_array){
        for(int i=0;i<first_array.used;i++){
                if (check_equal(first_array.get_string(i))){
                        add_string(first_array.get_string(i));
                }
        }
        return *this;
}

void filling_array(Array_string& first_array){
        char string[256];
        char choice;
        while (true){
                cout << "Do you want to continue?(y/n)" << endl;
                cin >> choice;
                if (choice!='n' && choice!='y')
                        continue;
                if (choice=='n')
                        break;
                cout << "Enter string please" << endl;
                cin >> string;
                first_array.add_string(string);
        }
}

int main(){
        int operation;
        Array_string first_array,second_array;
        cout << "select operation:" << endl;
        cout << "1-connect strings" << endl;
        cout << "2-connect strings without equal elements" << endl;
        cout << "3-exit" << endl;
        while (true){
                cout << "Select operation: ";
                cin >> operation;
                switch (operation){
                        case 1:
                                cout << "Fill in fisrt array" << endl;
                                filling_array(first_array);
                                cout << "Fill in second array" << endl;
                                filling_array(second_array);
                                cout << "Result: " << endl;
                                (first_array+second_array).print_array();
                                break;
                        case 2:
                                cout << "Fill in fisrt array" << endl;
                                filling_array(first_array);
                                cout << "Fill in second array" << endl;
                                filling_array(second_array);
                                cout << "Result: " << endl;
                                (first_array+=second_array).print_array();
                                break;
                        case 3:
                                return 0;
                        default:
                                cout << "Try again" << endl;
		}
	}
}
