#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
// REST
#include <cpprest/http_client.h>
#include <cpprest/json.h>

using namespace std;
using namespace web;
using namespace web::json;
using namespace web::http;
using namespace web::http::client;
using namespace ::pplx;


const string STRSALIDA = "Salida.txt";
const string STRURLDEV = "http://10.39.1.122:8080/code/Ordenes/crearOrdenes";
const string STRURLPRO = "http://172.16.4.132:8080/code/Ordenes/crearOrdenes";


template <class Container>
void split2(const std::string& str, Container& cont, char delim = ' ')
{
    stringstream ss(str);
    string token;
    while (std::getline(ss, token, delim)) {
        cont.push_back(token);
    }
}

// Procesamiento de la peticion
pplx::task<string> HTTPGetAsync(json::value peticion)
{
    http_client client(U("http://172.16.4.132:8186/code/Ordenes/"));

    http_request request(methods::POST);
    request.headers().set_content_type(L"application/json");
    request.set_request_uri(U("crearOrdenes"));
    request.set_body(peticion);


    return client
        .request(request)
        .then([](http_response response)
            {
                string retorno = "Error";
                //std::wostringstream stream;
                //stream.str(std::wstring());
                //stream << L"Content type: " << response.headers().content_type() << std::endl;
                //stream << L"Content length: " << response.headers().content_length() << L"bytes" << std::endl;
                //std::wcout << stream.str();

                if (response.status_code() == status_codes::OK)
                {
                    cout << "Respueta OK del servicio" << endl;
                    retorno = utility::conversions::to_utf8string(response.extract_string().get());
                }
                return retorno;
                
            });
}


int main(int argc, char** argv)
{
    // Variables
    ifstream ifs;
    ofstream ofs;
    char* nombreent;
    char* nombresal;
    int ilinea;
    string linea;
    string respuesta;
    vector<string> vseparado;


    // Procesamiento
    if (argc < 2) {
        cout << "Numero incorrecto de parametros" << endl;
        cout << "\n\tmasivopica [Archivo a Procesar]" << endl;
        cout << "PUJ -- 2020" << endl;
        return 1;
    }

    nombreent = new char[sizeof(char) * strlen(argv[1])];
    nombresal = new char[sizeof(char) * strlen(STRSALIDA.c_str())];

    strcpy_s(nombreent, strlen(argv[1]) + 1, argv[1]);
    strcpy_s(nombresal, strlen(STRSALIDA.c_str()) + 1, STRSALIDA.c_str());

    ifs.open(nombreent);
    ofs.open(nombresal);

    ilinea = 0;
    while (getline(ifs, linea)) {
        vseparado.clear();
        ilinea++;
        split2(linea, vseparado, ';');
        if (vseparado.size() != 6) {
            cout << "Linea " << ilinea << " no tiene la cantidad de campos requeridos -> [" << linea << "]" << endl;
            continue;
        }

        json::value putvalue;
        putvalue[U("ordenId")] = json::value::number(stoi(vseparado[0]));
        putvalue[U("nombreApellidoCliente")] = json::value::string(utility::conversions::usascii_to_utf16(vseparado[1]));
        putvalue[U("direccionCliente")] = json::value::string(utility::conversions::usascii_to_utf16(vseparado[2]));
        putvalue[U("ciudadCliente")] = json::value::string(utility::conversions::usascii_to_utf16(vseparado[3]));
        putvalue[U("paisCliente")] = json::value::string(utility::conversions::usascii_to_utf16(vseparado[4]));
        putvalue[U("nombreProveedorLogisticaEnvio")] = json::value::string(utility::conversions::usascii_to_utf16(vseparado[5]));

        json::value arreglo;
        arreglo[0] = putvalue;

        respuesta = HTTPGetAsync(arreglo).get();

        ofs << "[" << ilinea << "]" << endl;
        for (int i = 0; i < vseparado.size(); i++) {
            ofs << i << "->" << vseparado[i] << endl;
        }
        ofs << "Respuesta:" << respuesta << endl;
    }

    ifs.close();
    ofs.close();

    std::cout << "Proceso finalizado!\n";
}
