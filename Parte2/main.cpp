#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// ======================================================
// MÉTODO DE CODIFICACIÓN: Rotación de 3 bits
// ======================================================
unsigned char rotarDerecha(unsigned char valor, int n) {
    return (valor >> n) | (valor << (8 - n));
}
unsigned char rotarIzquierda(unsigned char valor, int n) {
    return (valor << n) | (valor >> (8 - n));
}

// ======================================================
// FUNCIONES DE ARCHIVOS
// ======================================================
string leerArchivo(string nombre) {
    ifstream archivo(nombre.c_str(), ios::binary);
    if (!archivo) throw string("No se pudo abrir el archivo ") + nombre;
    string contenido((istreambuf_iterator<char>(archivo)), istreambuf_iterator<char>());
    return contenido;
}

void guardarArchivo(string nombre, string datos) {
    ofstream archivo(nombre.c_str(), ios::binary);
    if (!archivo) throw string("No se pudo guardar el archivo ") + nombre;
    archivo << datos;
    archivo.close();
}

// ======================================================
// CODIFICAR / DECODIFICAR (solo método 1)
// ======================================================
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

// ======================================================
// FUNCIONES DE SISTEMA
// ======================================================
bool loginAdmin() {
    string codificado = leerArchivo("sudo.txt");
    string decodificado = decodificar(codificado);

    int i = decodificado.find(',');
    string userFile = decodificado.substr(0, i);
    string passFile = decodificado.substr(i + 1);

    string user, pass;
    cout << "Usuario administrador: ";
    cin >> user;
    cout << "Clave: ";
    cin >> pass;

    return (user == userFile && pass == passFile);
}

bool loginUsuario(string cedula, string clave) {
    string codificado = leerArchivo("usuarios.txt");
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

void registrarUsuario() {
    try {
        string codificado = leerArchivo("usuarios.txt");
        string decodificado = decodificar(codificado);

        string cedula, clave;
        cout << "Cedula nueva: ";
        cin >> cedula;
        cout << "Clave nueva: ";
        cin >> clave;

        string nuevo = cedula + "," + clave + ",0\n";
        decodificado += nuevo;

        guardarArchivo("usuarios.txt", codificar(decodificado));
        cout << "Usuario registrado correctamente.\n";
    } catch (...) {
        string cedula, clave;
        cout << "Cedula nueva: ";
        cin >> cedula;
        cout << "Clave nueva: ";
        cin >> clave;

        string nuevo = cedula + "," + clave + ",0\n";
        guardarArchivo("usuarios.txt", codificar(nuevo));
        cout << "Archivo creado y usuario registrado.\n";
    }
}

void registrarTransaccion(string tipo, string usuario) {
    string registro = usuario + "," + tipo + "\n";
    try {
        string cod = leerArchivo("transacciones.txt");
        string dec = decodificar(cod);
        dec += registro;
        guardarArchivo("transacciones.txt", codificar(dec));
    } catch (...) {
        guardarArchivo("transacciones.txt", codificar(registro));
    }
}

void menuUsuario(string cedula) {
    string codificado = leerArchivo("usuarios.txt");
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
                cout << "Saldo actual: " << saldo << endl;
                registrarTransaccion("Consulta", ced);
            } else if (op == 2) {
                long monto;
                cout << "Monto a retirar: ";
                cin >> monto;
                if (monto + 1000 <= saldo) {
                    saldo -= monto + 1000;
                    cout << "Retiro exitoso. Nuevo saldo: " << saldo << endl;
                    registrarTransaccion("Retiro", ced);
                } else {
                    cout << "Fondos insuficientes.\n";
                }
            }
            linea = ced + "," + pass + "," + to_string(saldo);
        }
        nuevo += linea + "\n";
    }
    guardarArchivo("usuarios.txt", codificar(nuevo));
}

// ======================================================
// PROGRAMA PRINCIPAL
// ======================================================
int main() {
    cout << "=== SISTEMA BANCARIO (Metodo: Rotacion de 3 bits) ===\n";

    int tipo;
    cout << "1. Admin\n2. Usuario\nOpcion: ";
    cin >> tipo;

    try {
        if (tipo == 1) {
            if (loginAdmin()) {
                cout << "Acceso de administrador correcto.\n";
                registrarUsuario();
                registrarTransaccion("LoginAdmin", "admin");
            } else {
                cout << "Credenciales incorrectas.\n";
            }
        } else if (tipo == 2) {
            string cedula, clave;
            cout << "Cedula: ";
            cin >> cedula;
            cout << "Clave: ";
            cin >> clave;
            if (loginUsuario(cedula, clave)) {
                cout << "Bienvenido usuario.\n";
                registrarTransaccion("LoginUsuario", cedula);
                menuUsuario(cedula);
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
