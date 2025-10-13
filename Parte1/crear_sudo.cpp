#include <iostream>
#include <fstream>
using namespace std;

unsigned char rotarDerecha(unsigned char valor, int n) {
    return (valor >> n) | (valor << (8 - n));
}

string codificar(string texto) {
    string resultado = texto;
    for (int i = 0; i < texto.size(); i++)
        resultado[i] = rotarDerecha(texto[i], 3);
    return resultado;
}

int main() {
    string admin = "admin,1234";
    string codificado = codificar(admin);

    ofstream out("sudo.txt", ios::binary);
    out << codificado;
    out.close();

    cout << "✅ Archivo sudo.txt creado y codificado correctamente." << endl;
    return 0;
}

