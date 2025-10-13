#include <iostream>
#include <fstream>
#include <string>
using namespace std;

//---------------------------------------------------------
// Convierte un caracter a su representacion binaria de 8 bits
//---------------------------------------------------------
string aBinario(unsigned char c) {
    string bin = "";
    for (int i = 7; i >= 0; i--) {
        if ((c >> i) & 1) bin += '1';
        else bin += '0';
    }
    return bin;
}

//---------------------------------------------------------
// METODO 1: Inversion segun reglas del enunciado
//---------------------------------------------------------
string metodo1(string bin, int n) {
    string resultado = "";
    for (int i = 0; i < (int)bin.size(); i += n) {
        string bloque = bin.substr(i, n);

        if (i == 0) {
            // Primer bloque: invertir todos los bits
            for (char &c : bloque) c = (c == '0') ? '1' : '0';
        } else {
            string anterior = bin.substr(i - n, n);
            int cont0 = 0, cont1 = 0;
            for (char c : anterior)
                (c == '0') ? cont0++ : cont1++;

            if (cont0 == cont1) {
                for (char &c : bloque) c = (c == '0') ? '1' : '0';
            }
            else if (cont0 > cont1) { // mas ceros → invierte cada 2 bits
                for (int j = 0; j + 1 < (int)bloque.size(); j += 2)
                    swap(bloque[j], bloque[j + 1]);
            }
            else { // mas unos → invierte cada 3 bits
                for (int j = 0; j + 2 < (int)bloque.size(); j += 3)
                    swap(bloque[j], bloque[j + 2]);
            }
        }
        resultado += bloque;
    }
    return resultado;
}

//---------------------------------------------------------
// METODO 2: Rotar a la izquierda cada bloque de n bits
//---------------------------------------------------------
string metodo2(string bin, int n) {
    string resultado = "";
    for (int i = 0; i < (int)bin.size(); i += n) {
        string bloque = bin.substr(i, n);
        if ((int)bloque.size() == n) {
            string rotado = bloque.substr(1) + bloque[0];
            resultado += rotado;
        } else {
            resultado += bloque;
        }
    }
    return resultado;
}

//---------------------------------------------------------
// PROGRAMA PRINCIPAL
//---------------------------------------------------------
int main() {
    try {
        string entrada, salida;
        int n, metodo;

        cout << "=== CODIFICADOR DE ARCHIVOS ===\n";
        cout << "Archivo de entrada: ";
        cin >> entrada;
        cout << "Archivo de salida: ";
        cin >> salida;
        cout << "Tamano de bloque (n): ";
        cin >> n;
        cout << "Metodo (1 o 2): ";
        cin >> metodo;

        if (n <= 0) throw string("El tamano de bloque debe ser mayor que 0.");
        if (metodo != 1 && metodo != 2) throw string("El metodo debe ser 1 o 2.");

        ifstream fin(entrada);
        if (!fin.is_open()) throw string("No se pudo abrir el archivo de entrada.");

        // Leer todo el archivo en un string
        string texto, linea;
        while (getline(fin, linea))
            texto += linea + '\n';
        fin.close();

        // Convertir cada caracter a binario
        string bin = "";
        for (char c : texto)
            bin += aBinario(c);

        // Aplicar metodo
        string resultado = (metodo == 1) ? metodo1(bin, n) : metodo2(bin, n);

        ofstream fout(salida);
        if (!fout.is_open()) throw string("No se pudo crear el archivo de salida.");
        fout << resultado;
        fout.close();

        cout << "\nArchivo codificado exitosamente en: " << salida << endl;
    }
    catch (string &error) {
        cerr << "Error: " << error << endl;
    }

    return 0;
}
