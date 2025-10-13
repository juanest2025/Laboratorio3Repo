#include <iostream>
#include <fstream>
#include <string>
using namespace std;

//---------------------------------------------------------
// Convierte un caracter a binario de 8 bits
//---------------------------------------------------------
string aBinario(unsigned char c) {
    string bin = "";
    for (int i = 7; i >= 0; i--) bin += ((c >> i) & 1) ? '1' : '0';
    return bin;
}

//---------------------------------------------------------
// Convierte un string de bits (múltiplos de 8) a texto
// Si el último byte está incompleto se ignora.
//---------------------------------------------------------
string binarioATexto(const string &bin) {
    string out = "";
    for (size_t i = 0; i + 7 < bin.size(); i += 8) {
        unsigned char val = 0;
        for (int b = 0; b < 8; ++b) {
            val = (val << 1) | (bin[i + b] == '1' ? 1u : 0u);
        }
        out += static_cast<char>(val);
    }
    return out;
}

//---------------------------------------------------------
// METODO 1: Inversion segun reglas (codificar)
//---------------------------------------------------------
string metodo1_codificar(const string &bin, int n) {
    string resultado = "";
    for (int i = 0; i < (int)bin.size(); i += n) {
        string bloque = bin.substr(i, n);

        if (i == 0) {
            // Primer bloque: invertir todos los bits
            for (char &c : bloque) c = (c == '0') ? '1' : '0';
        } else {
            string anterior = bin.substr(i - n, n);
            int cont0 = 0, cont1 = 0;
            for (char c : anterior) (c == '0') ? cont0++ : cont1++;

            if (cont0 == cont1) {
                for (char &c : bloque) c = (c == '0') ? '1' : '0';
            }
            else if (cont0 > cont1) { // más ceros → invierte cada 2 bits (swap pares)
                for (int j = 0; j + 1 < (int)bloque.size(); j += 2)
                    swap(bloque[j], bloque[j + 1]);
            }
            else { // más unos → invierte cada 3 bits (swap j <-> j+2)
                for (int j = 0; j + 2 < (int)bloque.size(); j += 3)
                    swap(bloque[j], bloque[j + 2]);
            }
        }
        resultado += bloque;
    }
    return resultado;
}

//---------------------------------------------------------
// METODO 1: Decodificar (usar bloques reconstruidos previos)
//---------------------------------------------------------
string metodo1_decodificar(const string &binEnc, int n) {
    string resultado = "";
    for (int i = 0; i < (int)binEnc.size(); i += n) {
        string bloqueEnc = binEnc.substr(i, n);
        string bloqueOrig = bloqueEnc; // aquí guardaremos el bloque original recuperado

        if (i == 0) {
            // Primer bloque: durante codificación se invirtió todo -> invertir para recuperar
            for (char &c : bloqueOrig) c = (c == '0') ? '1' : '0';
        } else {
            // Miramos el bloque original anterior (ya recuperado en 'resultado')
            string anteriorOrig = resultado.substr(i - n, n);
            int cont0 = 0, cont1 = 0;
            for (char c : anteriorOrig) (c == '0') ? cont0++ : cont1++;

            if (cont0 == cont1) {
                // Durante codificación se invirtió todo -> invertir para recuperar
                for (char &c : bloqueOrig) c = (c == '0') ? '1' : '0';
            } else if (cont0 > cont1) {
                // Durante codificación se hicieron swaps por pares (j<->j+1).
                // Esa operación es su propia inversa: aplicarla de nuevo recupera original.
                for (int j = 0; j + 1 < (int)bloqueOrig.size(); j += 2)
                    swap(bloqueOrig[j], bloqueOrig[j + 1]);
            } else {
                // cont1 > cont0 : swaps j <-> j+2 (también auto-inversa)
                for (int j = 0; j + 2 < (int)bloqueOrig.size(); j += 3)
                    swap(bloqueOrig[j], bloqueOrig[j + 2]);
            }
        }

        resultado += bloqueOrig;
    }
    return resultado;
}

//---------------------------------------------------------
// METODO 2: Rotacion a la izquierda (codificar)
//---------------------------------------------------------
string metodo2_codificar(const string &bin, int n) {
    string resultado = "";
    for (int i = 0; i < (int)bin.size(); i += n) {
        string bloque = bin.substr(i, n);
        if ((int)bloque.size() == n) {
            string rotado = bloque.substr(1) + bloque[0];
            resultado += rotado;
        } else resultado += bloque;
    }
    return resultado;
}

//---------------------------------------------------------
// METODO 2: Rotacion a la derecha (decodificar)
//---------------------------------------------------------
string metodo2_decodificar(const string &bin, int n) {
    string resultado = "";
    for (int i = 0; i < (int)bin.size(); i += n) {
        string bloque = bin.substr(i, n);
        if ((int)bloque.size() == n) {
            string rotado = bloque.substr(n - 1) + bloque.substr(0, n - 1);
            resultado += rotado;
        } else resultado += bloque;
    }
    return resultado;
}

//---------------------------------------------------------
// PROGRAMA PRINCIPAL
//---------------------------------------------------------
int main() {
    try {
        string entrada, salida;
        int n, metodo, opcion;

        cout << "=== CODIFICADOR / DECODIFICADOR ===\n";
        cout << "1. Codificar\n2. Decodificar\nOpcion: ";
        cin >> opcion;
        cout << "Archivo de entrada: ";
        cin >> entrada;
        cout << "Archivo de salida: ";
        cin >> salida;
        cout << "Tamano de bloque (n): ";
        cin >> n;
        cout << "Metodo (1=Inversion, 2=Rotacion): ";
        cin >> metodo;

        if (n <= 0) throw string("El tamano de bloque debe ser mayor que 0.");
        if (metodo != 1 && metodo != 2) throw string("El metodo debe ser 1 o 2.");
        if (opcion != 1 && opcion != 2) throw string("La opcion debe ser 1 o 2.");

        // Leer archivo completo sin modificar líneas (evitar manipular con + '\n' innecesario)
        ifstream fin(entrada, ios::binary);
        if (!fin.is_open()) throw string("No se pudo abrir el archivo de entrada.");

        string texto((istreambuf_iterator<char>(fin)), istreambuf_iterator<char>());
        fin.close();

        string resultadoBits;

        if (opcion == 1) { // CODIFICAR: convertir texto -> bin y aplicar método
            string bin = "";
            for (unsigned char c : texto) bin += aBinario(c);

            if (metodo == 1) resultadoBits = metodo1_codificar(bin, n);
            else resultadoBits = metodo2_codificar(bin, n);

            // Guardamos la cadena de bits como texto (solo '0' y '1')
            ofstream fout(salida, ios::binary);
            if (!fout.is_open()) throw string("No se pudo crear el archivo de salida.");
            fout << resultadoBits;
            fout.close();
        } else { // DECODIFICAR: entrada ya debe ser la cadena de bits ('0'/'1')
            string binEnc = texto;
            // eliminar posibles '\r' si existe (por archivos Windows)
            string tmp = "";
            for (char c : binEnc) if (c != '\r') tmp += c;
            binEnc = tmp;

            if (metodo == 1) resultadoBits = metodo1_decodificar(binEnc, n);
            else resultadoBits = metodo2_decodificar(binEnc, n);

            // Intentamos convertir binario a texto legible: si hay bytes completos, los recuperamos.
            string textoRecuperado = binarioATexto(resultadoBits);

            ofstream fout(salida, ios::binary);
            if (!fout.is_open()) throw string("No se pudo crear el archivo de salida.");
            fout << textoRecuperado;
            fout.close();
        }

        cout << "\nOperacion completada. Resultado guardado en: " << salida << endl;
    }
    catch (string &error) {
        cerr << "Error: " << error << endl;
    }
    return 0;
}
