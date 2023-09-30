#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include "igvInterfaz.h"

// Aplicación del patrón Singleton
igvInterfaz *igvInterfaz::_instancia = nullptr;

// Métodos públicos ----------------------------------------

/**
 * Método para acceder al objeto único de la clase, en aplicación del patrón de
 * diseño Singleton
 * @return Una referencia al objeto único de la clase
 */
igvInterfaz &igvInterfaz::getInstancia() {
    if (!_instancia) {
        _instancia = new igvInterfaz;
    }

    return *_instancia;
}

/**
 * Crea el mundo que se visualiza en la ventana
 */
void igvInterfaz::crear_mundo() {  // r tiene valor por defecto (0,0,0)
    // crear cámaras
    p0 = igvPunto3D(3.0, 2.0, 4);
    r = igvPunto3D(0, 0, 0);
    V = igvPunto3D(0, 1.0, 0);

    _instancia->camara.set(IGV_PARALELA, p0, r, V, -1 * 3, 1 * 3, -1 * 3, 1 * 3, 1, 200);

    // Las cámaras se han creado con valores por defecto de 60 grados de apertura
    // y ratio de aspecto 1
}

/**
 * Inicializa todos los parámetros para crear una ventana de visualización
 * @param argc Número de parámetros por línea de comandos al ejecutar la
 *             aplicación
 * @param argv Parámetros por línea de comandos al ejecutar la aplicación
 * @param _ancho_ventana Ancho inicial de la ventana de visualización
 * @param _alto_ventana Alto inicial de la ventana de visualización
 * @param _pos_X Coordenada X de la posición inicial de la ventana de
 *               visualización
 * @param _pos_Y Coordenada Y de la posición inicial de la ventana de
 *               visualización
 * @param _titulo Título de la ventana de visualización
 * @pre Se asume que todos los parámetros tienen valores válidos
 * @post Cambia el alto y ancho de ventana almacenado en el objeto
 */
void
igvInterfaz::configura_entorno(int argc, char **argv, int _ancho_ventana, int _alto_ventana, int _pos_X, int _pos_Y,
                               std::string _titulo) {  // inicialización de los atributos de la interfaz
    ancho_ventana = _ancho_ventana;
    alto_ventana = _alto_ventana;

    // inicialización de la ventana de visualización
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(_ancho_ventana, _alto_ventana);
    glutInitWindowPosition(_pos_X, _pos_Y);
    glutCreateWindow(_titulo.c_str());

    glEnable(GL_DEPTH_TEST); // activa el ocultamiento de superficies por z-buffer
    glClearColor(1.0, 1.0, 1.0, 0.0); // establece el color de fondo de la ventana

    glEnable(GL_LIGHTING); // activa la iluminacion de la escena
    glEnable(GL_NORMALIZE); // normaliza los vectores normales para calculo iluminacion

    crear_mundo(); // crea el mundo a visualizar en la ventana
}

/**
 * Método para visualizar la escena y esperar a eventos sobre la interfaz
 */
void igvInterfaz::inicia_bucle_visualizacion() {
    glutMainLoop(); // inicia el bucle de visualización de GLUT
}

/**
 * Método para control de eventos del teclado
 * @param key Código de la tecla pulsada
 * @param x Coordenada X de la posición del cursor del ratón en el momento del
 *          evento de teclado
 * @param y Coordenada Y de la posición del cursor del ratón en el momento del
 *          evento de teclado
 * @pre Se asume que todos los parámetros tienen valores válidos
 * @post Los atributos de la clase pueden cambiar, dependiendo de la tecla pulsada
 */
void igvInterfaz::keyboardFunc(unsigned char key, int x, int y) {
    /* IMPORTANTE: en la implementación de este método hay que cambiar convenientemente el estado
      de los objetos de la aplicación, pero no hacer llamadas directas a funciones de OpenGL */
    switch (key) {
        case 'p':
        case 'P':
            // _instancia->escena.set_ejes(!_instancia->escena.get_ejes());
            if (_instancia->camara.getTipo() == IGV_PARALELA) {
                _instancia->camara.setTipo(IGV_PERSPECTIVA);
            } else if (_instancia->camara.getTipo() == IGV_PERSPECTIVA) {
                _instancia->camara.setTipo(IGV_PARALELA);
            }
            _instancia->camara.aplicar();
            break;
        case 'v':
        case 'V':
            switch (_instancia->getVistaActual()) {
                case PLANTA:
                    _instancia->setVistaActual(ALZADO);
                    _instancia->camara.set(igvPunto3D(0, 0, 5), igvPunto3D(0, 0, 0), igvPunto3D(0, 1, 0));
                    break;
                case ALZADO:
                    _instancia->setVistaActual(PERFIL);
                    _instancia->camara.set(igvPunto3D(5, 0, 0), igvPunto3D(0, 0, 0), igvPunto3D(0, 1, 0));
                    break;
                case PERFIL:
                    _instancia->setVistaActual(OTRA);
                    _instancia->camara.set(igvPunto3D(3, 2, 4), igvPunto3D(0, 0, 0), igvPunto3D(0, 1, 0));
                    break;
                case OTRA:
                    _instancia->setVistaActual(PLANTA);
                    _instancia->camara.set(igvPunto3D(0, 5, 0), igvPunto3D(0, 0, 0), igvPunto3D(1, 0, 0));
                    break;
            }
            _instancia->camara.aplicar();
            break;
        case '+':
            _instancia->camara.zoom(-0.05);
            _instancia->camara.aplicar();
            break;
        case '-':
            _instancia->camara.zoom(0.05);
            _instancia->camara.aplicar();
            break;
        case 'n':
            _instancia->camara.setZnear(_instancia->camara.getZnear() + 0.2);
            if(_instancia->camara.getZnear() >= _instancia->camara.getZfar())
                _instancia->camara.setZnear(_instancia->camara.getZfar() - 0.2);
            _instancia->camara.aplicar();
            break;
        case 'N':
            _instancia->camara.setZnear(_instancia->camara.getZnear() - 0.2);
            if(_instancia->camara.getZnear() <= 0)
                _instancia->camara.setZnear(0.2);
            _instancia->camara.aplicar();
            break;
        case '4':
            _instancia->vistaCuadruple = !_instancia->vistaCuadruple;  // Invertir el indicador
            break;
        case 'e': // activa/desactiva la visualización de los ejes
            _instancia->escena.set_ejes(!_instancia->escena.get_ejes());
            break;
        case 27: // tecla de escape para SALIR
            exit(1);
            break;
    }
    glutPostRedisplay(); // renueva el contenido de la ventana de vision y redibuja la escena
}

/**
 * Método que define la cámara de visión y el viewport. Se llama automáticamente
 * cuando se cambia el tamaño de la ventana.
 * @param w Nuevo ancho de la ventana
 * @param h Nuevo alto de la ventana
 * @pre Se asume que todos los parámetros tienen valores válidos
 */
void igvInterfaz::reshapeFunc(int w, int h) {  // dimensiona el viewport al nuevo ancho y alto de la ventana
    // guardamos valores nuevos de la ventana de visualizacion
    _instancia->set_ancho_ventana(w);
    _instancia->set_alto_ventana(h);

    // establece los parámetros de la cámara y de la proyección
    _instancia->camara.aplicar();
}

void igvInterfaz::displayFunc() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int ancho = _instancia->get_ancho_ventana();
    int alto = _instancia->get_alto_ventana();

    if (_instancia->vistaCuadruple) {
        // Vista 1
        glViewport(0, alto / 2, ancho / 2, alto / 2);
        _instancia->escena.visualizar();

        // Vista 2
        glViewport(ancho / 2, alto / 2, ancho / 2, alto / 2);
        _instancia->escena.visualizar();

        // Vista 3
        glViewport(0, 0, ancho / 2, alto / 2);
        _instancia->escena.visualizar();

        // Vista 4
        glViewport(ancho / 2, 0, ancho / 2, alto / 2);
        _instancia->escena.visualizar();
    } else {
        glViewport(0, 0, ancho, alto);
        _instancia->escena.visualizar();
    }

    glutSwapBuffers();
}

/**
 * Método para inicializar los callbacks GLUT
 */
void igvInterfaz::inicializa_callbacks() {
    glutKeyboardFunc(keyboardFunc);
    glutReshapeFunc(reshapeFunc);
    glutDisplayFunc(displayFunc);
}

/**
 * Método para consultar el ancho de la ventana de visualización
 * @return El valor almacenado como ancho de la ventana de visualización
 */
int igvInterfaz::get_ancho_ventana() {
    return ancho_ventana;
}

/**
 * Método para consultar el alto de la ventana de visualización
 * @return El valor almacenado como alto de la ventana de visualización
 */
int igvInterfaz::get_alto_ventana() {
    return alto_ventana;
}

/**
 * Método para cambiar el ancho de la ventana de visualización
 * @param _ancho_ventana Nuevo valor para el ancho de la ventana de visualización
 * @pre Se asume que el parámetro tiene un valor válido
 * @post El ancho de ventana almacenado en la aplicación cambia al nuevo valor
 */
void igvInterfaz::set_ancho_ventana(int _ancho_ventana) {
    ancho_ventana = _ancho_ventana;
}

/**
 * Método para cambiar el alto de la ventana de visualización
 * @param _alto_ventana Nuevo valor para el alto de la ventana de visualización
 * @pre Se asume que el parámetro tiene un valor válido
 * @post El alto de ventana almacenado en la aplicación cambia al nuevo valor
 */
void igvInterfaz::set_alto_ventana(int _alto_ventana) {
    alto_ventana = _alto_ventana;
}

Vista igvInterfaz::getVistaActual() const {
    return vistaActual;
}

void igvInterfaz::setVistaActual(Vista vistaActual) {
    igvInterfaz::vistaActual = vistaActual;
}

