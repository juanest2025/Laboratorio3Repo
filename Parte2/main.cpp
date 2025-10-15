#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// ----- Método de codificación: rotación de 3 bits -----
unsigned char rotarDerecha(unsigned char valor, int n) {
    return (valor >> n) | (valor << (8 - n));
}
unsigned char rotarIzquierda(unsigned char valor, int n) {
    return (valor << n) | (valor >> (8 - n));
}

// ----- Funciones para manejo de archivos -----
string leerArchivo(string nombre) {
    ifstream archivo(nombre.c_str(), ios::binary);
    if (!archivo) throw string("No se pudo abrir el archivo ") + nombre;
    string contenido = "", linea;
    while (getline(archivo, linea)) contenido += linea + "\n";
    archivo.close();
    return contenido;
}

void guardarArchivo(string nombre, string datos) {
    ofstream archivo(nombre.c_str(), ios::binary);
    if (!archivo) throw string("No se pudo guardar el archivo ") + nombre;
    archivo << datos;
    archivo.close();
}

// ----- Codificar / Decodificar -----
string codificar(string texto) {
    string resultado = texto;
    for (int i = 0; i < texto.size(); i++)
        resultado[i] = rotarDerecha(texto[i], 3);
    return resultado;
}

string decodificar(string texto) {
    string resultado = texto;
    for (int i = 0; i < texto.size(); i++)
        resultado[i] = rotarIzquierda(texto[i], 3);
    return resultado;
}

// ----- Funciones de sistema -----
bool loginAdmin(string archivoSudo) {
    string codificado = leerArchivo(archivoSudo);
    string decodificado = decodificar(codificado);

    int i = decodificado.find(',');
    string userFile = decodificado.substr(0, i);
    string passFile = decodificado.substr(i + 1);
    cout << userFile << "," << passFile << endl;
    string user, pass;
    cout << "Usuario administrador: ";
    cin >> user;
    cout << "Clave: ";
    cin >> pass;

    return (user == userFile && pass == passFile);
}

bool loginUsuario(string archivoUsuarios, string cedula, string clave) {
    string codificado = leerArchivo(archivoUsuarios);
    string decodificado = decodificar(codificado);

    int i = 0;
    while (i < decodificado.size()) {
        string linea = "";
        while (i < decodificado.size() && decodificado[i] != '\n') {
            linea += decodificado[i];
            i++;
        }
        i++;

        int p1 = linea.find(',');
        int p2 = linea.find(',', p1 + 1);
        string ced = linea.substr(0, p1);
        string pass = linea.substr(p1 + 1, p2 - p1 - 1);

        if (ced == cedula && pass == clave) return true;
    }
    return false;
}

void registrarUsuario(string archivoUsuarios) {
    try {
        string codificado = leerArchivo(archivoUsuarios);
        string decodificado = decodificar(codificado);

        string cedula, clave;
        long saldo;
        cout << "Cedula nueva: ";
        cin >> cedula;
        cout << "Clave nueva: ";
        cin >> clave;
        cout << "Saldo inicial (COP): ";
        cin >> saldo;

        string nuevo = cedula + "," + clave + "," + to_string(saldo) + "\n";
        decodificado += nuevo;

        guardarArchivo(archivoUsuarios, codificar(decodificado));
        cout << "Usuario registrado correctamente con saldo inicial de " << saldo << " COP.\n";
    } catch (...) {
        string cedula, clave;
        long saldo;
        cout << "Cedula nueva: ";
        cin >> cedula;
        cout << "Clave nueva: ";
        cin >> clave;
        cout << "Saldo inicial (COP): ";
        cin >> saldo;

        string nuevo = cedula + "," + clave + "," + to_string(saldo) + "\n";
        guardarArchivo(archivoUsuarios, codificar(nuevo));
        cout << "Archivo creado y usuario registrado con saldo inicial de " << saldo << " COP.\n";
    }
}

void registrarTransaccion(string archivoTrans, string tipo, string usuario) {
    string registro = usuario + "," + tipo + "\n";
    try {
        string cod = leerArchivo(archivoTrans);
        string dec = decodificar(cod);
        dec += registro;
        guardarArchivo(archivoTrans, codificar(dec));
    } catch (...) {
        guardarArchivo(archivoTrans, codificar(registro));
    }
}

void menuUsuario(string archivoUsuarios, string archivoTrans, string cedula) {
    string codificado = leerArchivo(archivoUsuarios);
    string decodificado = decodificar(codificado);

    string nuevo = "";
    int i = 0;
    while (i < decodificado.size()) {
        string linea = "";
        while (i < decodificado.size() && decodificado[i] != '\n') {
            linea += decodificado[i];
            i++;
        }
        i++;

        int p1 = linea.find(',');
        int p2 = linea.find(',', p1 + 1);
        string ced = linea.substr(0, p1);
        string pass = linea.substr(p1 + 1, p2 - p1 - 1);
        string saldoStr = linea.substr(p2 + 1);
        long saldo = stol(saldoStr);

        if (ced == cedula) {
            int op;
            cout << "\n1. Consultar saldo\n2. Retirar dinero\nOpcion: ";
            cin >> op;

            if (op == 1) {
                saldo -= 1000;
                cout << "Saldo actual: " << saldo << " COP\n";
                registrarTransaccion(archivoTrans, "Consulta", ced);
            } else if (op == 2) {
                long monto;
                cout << "Monto a retirar: ";
                cin >> monto;
                if (monto + 1000 <= saldo) {
                    saldo -= monto + 1000;
                    cout << "Retiro exitoso. Nuevo saldo: " << saldo << " COP\n";
                    registrarTransaccion(archivoTrans, "Retiro", ced);
                } else {
                    cout << "Fondos insuficientes.\n";
                }
            }
            linea = ced + "," + pass + "," + to_string(saldo);
        }
        nuevo += linea + "\n";
    }
    guardarArchivo(archivoUsuarios, codificar(nuevo));
}

// ----- Programa principal -----
int main(int argc, char* argv[]) {
    if (argc < 4) {
        cout << "Uso: " << argv[0] << " <sudo.txt> <usuarios.txt> <transacciones.txt>\n";
        return 1;
    }

    string archivoSudo = argv[1];
    string archivoUsuarios = argv[2];
    string archivoTrans = argv[3];

    cout << "--- SISTEMA BANCARIO (metodo: rotacion de 3 bits) ---\n";
    int tipo;
    cout << "1. Admin\n2. Usuario\nOpcion: ";
    cin >> tipo;

    try {
        if (tipo == 1) {
            if (loginAdmin(archivoSudo)) {
                cout << "Acceso de administrador correcto.\n";
                registrarUsuario(archivoUsuarios);
                registrarTransaccion(archivoTrans, "LoginAdmin", "admin");
            } else {
                cout << "Credenciales incorrectas.\n";
            }
        } else if (tipo == 2) {
            string cedula, clave;
            cout << "Cedula: ";
            cin >> cedula;
            cout << "Clave: ";
            cin >> clave;
            if (loginUsuario(archivoUsuarios, cedula, clave)) {
                cout << "Bienvenido usuario.\n";
                registrarTransaccion(archivoTrans, "LoginUsuario", cedula);
                menuUsuario(archivoUsuarios, archivoTrans, cedula);
            } else {
                cout << "Datos incorrectos.\n";
            }
        } else {
            cout << "Opcion invalida.\n";
        }
    } catch (string e) {
        cout << "Error: " << e << endl;
    }

    return 0;
}


