"""
Testes dos módulos pratos.c e restaurantes.c via ctypes.

Para rodar:
    1. Instale pytest:   pip install pytest
    2. Execute:          pytest test_modulos.py -v

O arquivo compila automaticamente os módulos C antes de rodar os testes.
Nenhuma classe Python é utilizada neste módulo.
"""

import pytest
import ctypes
import json
import os
import platform
import subprocess

# ================================================================
# Compilação automática
# ================================================================

_DIR = os.path.dirname(os.path.abspath(__file__))

if platform.system() == "Windows":
    _DLL = os.path.join(_DIR, "modulos.dll")
    _CMD = [
        "gcc", "-shared", "-o", _DLL,
        os.path.join(_DIR, "pratos.c"),
        os.path.join(_DIR, "restaurantes.c"),
        os.path.join(_DIR, "acessores.c"),
    ]
else:
    _DLL = os.path.join(_DIR, "modulos.so")
    _CMD = [
        "gcc", "-shared", "-fPIC", "-o", _DLL,
        os.path.join(_DIR, "pratos.c"),
        os.path.join(_DIR, "restaurantes.c"),
        os.path.join(_DIR, "acessores.c"),
    ]

_compilacao = subprocess.run(_CMD, capture_output=True, text=True)
if _compilacao.returncode != 0:
    raise RuntimeError(f"Falha ao compilar módulos C:\n{_compilacao.stderr}")

# ================================================================
# Carregamento da biblioteca
# ================================================================

lib = ctypes.CDLL(_DLL)

# --- funções principais ---
lib.initPratos.argtypes        = [ctypes.c_char_p]
lib.initPratos.restype         = None
lib.savePratos.argtypes        = [ctypes.c_char_p]
lib.savePratos.restype         = None
lib.initRestaurantes.argtypes  = [ctypes.c_char_p]
lib.initRestaurantes.restype   = None

lib.getPratos.argtypes         = [ctypes.c_int]
lib.getPratos.restype          = ctypes.c_void_p

lib.getListaPratos.argtypes    = [ctypes.c_char_p]
lib.getListaPratos.restype     = ctypes.c_void_p

lib.getMenu.argtypes           = [ctypes.c_longlong]
lib.getMenu.restype            = ctypes.c_void_p

lib.getFeedPratos.argtypes     = []
lib.getFeedPratos.restype      = ctypes.c_void_p

lib.getListaRest.argtypes      = [ctypes.c_char_p]
lib.getListaRest.restype       = ctypes.c_void_p

lib.getFeedRest.argtypes       = []
lib.getFeedRest.restype        = ctypes.c_void_p

lib.freeListaPratos.argtypes   = [ctypes.c_void_p]
lib.freeListaPratos.restype    = None
lib.freeListaRest.argtypes     = [ctypes.c_void_p]
lib.freeListaRest.restype      = None

# --- acessores de Prato ---
lib.prato_id.argtypes          = [ctypes.c_void_p]
lib.prato_id.restype           = ctypes.c_int
lib.prato_nome.argtypes        = [ctypes.c_void_p]
lib.prato_nome.restype         = ctypes.c_char_p
lib.prato_descricao.argtypes   = [ctypes.c_void_p]
lib.prato_descricao.restype    = ctypes.c_char_p
lib.prato_cnpj.argtypes        = [ctypes.c_void_p]
lib.prato_cnpj.restype         = ctypes.c_longlong
lib.prato_endereco.argtypes    = [ctypes.c_void_p]
lib.prato_endereco.restype     = ctypes.c_char_p

# --- acessores de ListaPratos ---
lib.lista_pratos_qtd.argtypes  = [ctypes.c_void_p]
lib.lista_pratos_qtd.restype   = ctypes.c_int
lib.lista_pratos_item.argtypes = [ctypes.c_void_p, ctypes.c_int]
lib.lista_pratos_item.restype  = ctypes.c_void_p

# --- acessores de Restaurante ---
lib.rest_cnpj.argtypes         = [ctypes.c_void_p]
lib.rest_cnpj.restype          = ctypes.c_longlong
lib.rest_nome.argtypes         = [ctypes.c_void_p]
lib.rest_nome.restype          = ctypes.c_char_p
lib.rest_endereco.argtypes     = [ctypes.c_void_p]
lib.rest_endereco.restype      = ctypes.c_char_p

# --- acessores de ListaRest ---
lib.lista_rest_qtd.argtypes    = [ctypes.c_void_p]
lib.lista_rest_qtd.restype     = ctypes.c_int
lib.lista_rest_item.argtypes   = [ctypes.c_void_p, ctypes.c_int]
lib.lista_rest_item.restype    = ctypes.c_void_p

# ================================================================
# Wrappers Python
#
# Convenção de retorno:
#   dict / list  → dados encontrados
#   None         → parâmetro nulo ou item inexistente
#   {} / []      → sem parâmetro (parâmetro ausente)
# ================================================================

_AUSENTE = object()  # sentinela para "sem parâmetro"


def get_pratos(id_prato=_AUSENTE):
    """Retorna: dict | None | {}"""
    if id_prato is _AUSENTE or id_prato is None:
        return {}
    ptr = lib.getPratos(id_prato)
    if ptr is None:
        return None
    return {
        "IDPrato":    lib.prato_id(ptr),
        "nome_prato": lib.prato_nome(ptr).decode("utf-8"),
        "descricao":  lib.prato_descricao(ptr).decode("utf-8"),
        "CNPJ_rest":  lib.prato_cnpj(ptr),
        "endereco":   lib.prato_endereco(ptr).decode("utf-8"),
    }


def get_lista_pratos(nome_prato=_AUSENTE):
    """Retorna: list[dict] | None | []"""
    if nome_prato is _AUSENTE:
        return []
    if nome_prato is None:
        return None
    ptr = lib.getListaPratos(nome_prato.encode("utf-8"))
    if ptr is None:
        return None
    resultado = []
    for i in range(lib.lista_pratos_qtd(ptr)):
        item = lib.lista_pratos_item(ptr, i)
        resultado.append({
            "IDPrato":    lib.prato_id(item),
            "nome_prato": lib.prato_nome(item).decode("utf-8"),
        })
    lib.freeListaPratos(ptr)
    return resultado


def get_menu(cnpj=_AUSENTE):
    """Retorna: list[dict] | None | []"""
    if cnpj is _AUSENTE or cnpj is None:
        return []
    ptr = lib.getMenu(cnpj)
    if ptr is None:
        return None
    resultado = []
    for i in range(lib.lista_pratos_qtd(ptr)):
        item = lib.lista_pratos_item(ptr, i)
        resultado.append({
            "IDPrato":    lib.prato_id(item),
            "nome_prato": lib.prato_nome(item).decode("utf-8"),
        })
    lib.freeListaPratos(ptr)
    return resultado


def get_feed_pratos():
    """Retorna: list[dict] com MAX_FEED pratos | None"""
    ptr = lib.getFeedPratos()
    if ptr is None:
        return None
    resultado = []
    for i in range(lib.lista_pratos_qtd(ptr)):
        item = lib.lista_pratos_item(ptr, i)
        resultado.append({
            "IDPrato":    lib.prato_id(item),
            "nome_prato": lib.prato_nome(item).decode("utf-8"),
        })
    lib.freeListaPratos(ptr)
    return resultado


def get_lista_rest(nome_rest=_AUSENTE):
    """Retorna: list[dict] | None | []"""
    if nome_rest is _AUSENTE:
        return []
    if nome_rest is None:
        return None
    ptr = lib.getListaRest(nome_rest.encode("utf-8"))
    if ptr is None:
        return None
    resultado = []
    for i in range(lib.lista_rest_qtd(ptr)):
        item = lib.lista_rest_item(ptr, i)
        resultado.append({
            "CNPJ":      lib.rest_cnpj(item),
            "endereco":  lib.rest_endereco(item).decode("utf-8"),
            "nome_rest": lib.rest_nome(item).decode("utf-8"),
        })
    lib.freeListaRest(ptr)
    return resultado


def get_feed_rest():
    """Retorna: list[dict] com MAX_FEED_REST restaurantes | None"""
    ptr = lib.getFeedRest()
    if ptr is None:
        return None
    resultado = []
    for i in range(lib.lista_rest_qtd(ptr)):
        item = lib.lista_rest_item(ptr, i)
        resultado.append({
            "CNPJ":      lib.rest_cnpj(item),
            "nome_rest": lib.rest_nome(item).decode("utf-8"),
        })
    lib.freeListaRest(ptr)
    return resultado


# ================================================================
# Helpers para geração de dados de teste
# ================================================================

def _gerar_pratos(n, cnpj_base=10_000_000_000_000):
    return {
        "pratos": [
            {
                "IDPrato":    i,
                "nome_prato": f"Prato Teste {i}",
                "descricao":  f"Descricao prato {i}",
                "CNPJ_rest":  cnpj_base + (i % 8),
                "endereco":   "Rua de Teste, 0",
            }
            for i in range(1, n + 1)
        ]
    }


def _gerar_restaurantes(n, cnpj_base=10_000_000_000_000):
    return {
        "restaurantes": [
            {
                "CNPJ":      cnpj_base + i,
                "endereco":  f"Rua Teste, {i * 10}",
                "nome_rest": f"Restaurante Teste {i}",
            }
            for i in range(n)
        ]
    }


def _salvar(path, dados):
    with open(path, "w", encoding="utf-8") as f:
        json.dump(dados, f, ensure_ascii=False)


# ================================================================
# Fixtures
# ================================================================

@pytest.fixture
def pratos_populados(tmp_path):
    """25 pratos carregados — suficiente para getFeedPratos (MAX_FEED=20)."""
    arq = str(tmp_path / "pratos.json")
    _salvar(arq, _gerar_pratos(25))
    lib.initPratos(arq.encode())


@pytest.fixture
def pratos_vazios(tmp_path):
    """Nenhum prato carregado."""
    arq = str(tmp_path / "pratos_vazios.json")
    _salvar(arq, {"pratos": []})
    lib.initPratos(arq.encode())


@pytest.fixture
def restaurantes_populados(tmp_path):
    """
    8 restaurantes + 16 pratos (2 por restaurante).
    initPratos é chamado antes de initRestaurantes conforme exigido.
    """
    arq_p = str(tmp_path / "pratos.json")
    arq_r = str(tmp_path / "restaurantes.json")
    _salvar(arq_p, _gerar_pratos(16))
    _salvar(arq_r, _gerar_restaurantes(8))
    lib.initPratos(arq_p.encode())
    lib.initRestaurantes(arq_r.encode())


@pytest.fixture
def restaurantes_vazios(tmp_path):
    """Nenhum restaurante carregado."""
    arq_p = str(tmp_path / "pratos_vazio.json")
    arq_r = str(tmp_path / "restaurantes_vazios.json")
    _salvar(arq_p, {"pratos": []})
    _salvar(arq_r, {"restaurantes": []})
    lib.initPratos(arq_p.encode())
    lib.initRestaurantes(arq_r.encode())


# ================================================================
# Testes — Módulo Restaurantes — getListaRest
# ================================================================

def test_getListaRest_c1_nome_existente(restaurantes_populados):
    """C1: busca por nome existente retorna lista_rest com os restaurantes."""
    resultado = get_lista_rest("Restaurante")
    assert isinstance(resultado, list)
    assert len(resultado) > 0
    assert all("nome_rest" in r and "CNPJ" in r for r in resultado)


def test_getListaRest_c2_nome_nulo(restaurantes_populados):
    """C2: nome nulo retorna None."""
    resultado = get_lista_rest(None)
    assert resultado is None


def test_getListaRest_c3_sem_parametro(restaurantes_populados):
    """C3: sem parâmetro retorna lista vazia."""
    resultado = get_lista_rest()
    assert resultado == []


# ================================================================
# Testes — Módulo Restaurantes — getFeedRest
# ================================================================

def test_getFeedRest_c1_lista_suficiente(restaurantes_populados):
    """C1: com 8 restaurantes (> 6), retorna lista com exatamente 6 itens únicos."""
    resultado = get_feed_rest()
    assert isinstance(resultado, list)
    assert len(resultado) == 6
    cnpjs = [r["CNPJ"] for r in resultado]
    assert len(cnpjs) == len(set(cnpjs)), "Restaurantes do feed devem ser únicos"


def test_getFeedRest_c2_sem_restaurantes(restaurantes_vazios):
    """C2: sem restaurantes salvos, retorna None."""
    resultado = get_feed_rest()
    assert resultado is None


# ================================================================
# Testes — Módulo Pratos — getPratos
# ================================================================

def test_getPratos_c1_id_existente(pratos_populados):
    """C1: IDPrato existente retorna dicionário completo do prato."""
    resultado = get_pratos(1)
    assert isinstance(resultado, dict)
    assert resultado["IDPrato"] == 1
    assert "nome_prato" in resultado
    assert "descricao" in resultado


def test_getPratos_c2_id_inexistente(pratos_populados):
    """C2: IDPrato não existente retorna None."""
    resultado = get_pratos(9999)
    assert resultado is None


def test_getPratos_c3_sem_parametro(pratos_populados):
    """C3: sem parâmetro retorna dicionário vazio."""
    resultado = get_pratos()
    assert resultado == {}


# ================================================================
# Testes — Módulo Pratos — getListaPratos
# ================================================================

def test_getListaPratos_c1_nome_existente(pratos_populados):
    """C1: nome existente retorna lista de pratos com os campos esperados."""
    resultado = get_lista_pratos("Prato")
    assert isinstance(resultado, list)
    assert len(resultado) > 0
    assert all("IDPrato" in p and "nome_prato" in p for p in resultado)


def test_getListaPratos_c2_nome_nulo(pratos_populados):
    """C2: nome nulo retorna None."""
    resultado = get_lista_pratos(None)
    assert resultado is None


def test_getListaPratos_c3_sem_parametro(pratos_populados):
    """C3: sem parâmetro retorna lista vazia."""
    resultado = get_lista_pratos()
    assert resultado == []


# ================================================================
# Testes — Módulo Pratos — getMenu
# ================================================================

def test_getMenu_c1_cnpj_existente(pratos_populados):
    """C1: CNPJ com pratos cadastrados retorna lista de pratos do restaurante."""
    # CNPJ base+1 tem pratos onde IDPrato % 8 == 1 → IDs 1, 9, 17, 25
    resultado = get_menu(10_000_000_000_001)
    assert isinstance(resultado, list)
    assert len(resultado) > 0
    assert all("IDPrato" in p for p in resultado)


def test_getMenu_c2_cnpj_inexistente(pratos_populados):
    """C2: CNPJ sem nenhum prato cadastrado retorna None."""
    resultado = get_menu(99_999_999_999_999)
    assert resultado is None


def test_getMenu_c3_sem_parametro(pratos_populados):
    """C3: sem parâmetro retorna lista vazia."""
    resultado = get_menu()
    assert resultado == []


# ================================================================
# Testes — Módulo Pratos — getFeedPratos
# ================================================================

def test_getFeedPratos_c1_lista_suficiente(pratos_populados):
    """C1: com 25 pratos (> 20), retorna lista com exatamente 20 itens únicos."""
    resultado = get_feed_pratos()
    assert isinstance(resultado, list)
    assert len(resultado) == 20
    ids = [p["IDPrato"] for p in resultado]
    assert len(ids) == len(set(ids)), "Pratos do feed devem ser únicos"


def test_getFeedPratos_c2_sem_pratos(pratos_vazios):
    """C2: sem pratos salvos, retorna None."""
    resultado = get_feed_pratos()
    assert resultado is None
