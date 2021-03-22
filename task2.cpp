#include <iostream>
  
using namespace std;

class Matrix{
        float **array;
        int rows;
        int columns;
public:
        Matrix(const int numRows,const int numColumns);
        Matrix(const Matrix& matrix1);
        ~Matrix();
        void inputMatrix();
        void printMatrix() const;
        inline int getRows() const{return rows;}
        inline int getColumns() const{return columns;}
        friend Matrix operator*(const Matrix& matrix1, const Matrix& matrix2);
        friend Matrix operator*(const Matrix& matrix1, float alpha);
        friend Matrix operator*(float alpha,const Matrix& matrix1);
};

Matrix::Matrix(const int numRows=1,const int numColumns=1){
        rows=numRows;
        columns=numColumns;
        array=new float*[rows];
        for(int i=0;i<rows;i++) array[i]=new float[columns];
}

Matrix::Matrix(const Matrix& matrix1){
        rows=matrix1.rows;
        columns=matrix1.columns;
        array=new float*[rows];
        for(int i=0;i<rows;i++){
                array[i]=new float[columns];
                for(int j=0;j<columns;j++){
                        array[i][j]=matrix1.array[i][j];
                }
        }
}

Matrix::~Matrix(){
        for(int i=0;i<rows;i++) delete []array[i];
        delete []array;
}

void Matrix::inputMatrix(){
        for (int i=0;i<rows;i++){
                for(int j=0;j<columns;j++){
                        cin>>array[i][j];
                }
        }
}

void Matrix::printMatrix() const{
        for (int i=0;i<rows;i++){
                for(int j=0;j<columns;j++){
                        cout<<array[i][j]<<endl;
                }
        }
}

Matrix operator*(const Matrix& matrix1,const Matrix& matrix2){
        if ((matrix1.rows)!=(matrix2.columns)) throw "Error";
        Matrix result(matrix1.rows,matrix2.columns);
        for (int i=0;i<(matrix1.rows);i++){
                for(int j=0;j<(matrix2.columns);j++){
                        result.array[i][j]=0;
                        for (int k=0;k<(matrix1.columns);k++){
                                result.array[i][j]+=matrix1.array[i][k]*matrix2.array[k][j];
                        }
                }
        }
        return result;
}

Matrix operator*(const Matrix& matrix1, float alpha){
        Matrix result(matrix1.rows,matrix1.columns);
        for(int i=0;i<matrix1.rows;i++){
                for(int j=0;j<matrix1.columns;j++){
                        result.array[i][j]=matrix1.array[i][j]*alpha;
                }
        }
        return result;
}

Matrix operator*(float alpha,const Matrix& matrix1){
        Matrix result(matrix1.rows,matrix1.columns);
        for(int i=0;i<matrix1.rows;i++){
                for(int j=0;j<matrix1.columns;j++){
                        result.array[i][j]=matrix1.array[i][j]*alpha;
                }
        }
        return result;
}

int main(){
        int rows1,columns1,rows2,columns2,choice,alpha;
        cout<<"select operation:\n";
        cout<<"1-multiply two matrix\n";
        cout<<"2-multiply matrix and number\n";
        cout<<"3-exit\n";
        Matrix *first,*second;
        try{
        while (true){
                cout<<"choice:";
                cin>>choice;
                switch (choice){
                        case 1:
                                cout<<"enter number of rows of first matrix:";
                                cin>>rows1;
                                cout<<"enter number of columns of first matrix:";
                                cin>>columns1;
                                first=new Matrix(rows1,columns1);
                                cout<<"enter elements of matrix"<<endl;
                                (*first).inputMatrix();
                                cout<<"enter number of rows of second matrix:";
                                cin>>rows2;
                                cout<<"enter number of columns of second matrix:";
                                cin>>columns2;
                                second=new Matrix(rows2,columns2);
                                cout<<"enter elements of matrix"<<endl;
                                (*second).inputMatrix();
                                ((*first)*(*second)).printMatrix();
                                delete first;
                                delete second;
                                break;
                        case 2:
                                cout<<"enter number of rows of matrix:";
                                cin>>rows1;
                                cout<<"enter number of columns of matrix:";
                                cin>>columns1;
                                first=new Matrix(rows1,columns1);
                                cout<<"enter elements of matrix"<<endl;
                                (*first).inputMatrix();
                                cout<<"enter number:";
                                cin>>alpha;
                                ((*first)*alpha).printMatrix();
                                cout<<"------------------"<<endl;
                                (alpha*(*first)).printMatrix();
                                delete first;
                                break;
                        case 3:
                                return 0;
                        default:
                                cout<<"Try again"<<endl;
                }
        }
        }
        catch(const char *reaction){
            cerr << reaction << endl;
        }
        return 0;
}
