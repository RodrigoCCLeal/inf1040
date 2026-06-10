"""
Testes dos módulos pratos.c e restaurantes.c via ctypes.

Para rodar:
    1. Instale pytest:   pip install pytest
    2. Execute:          pytest test_modulos.py -v

O arquivo compila automaticamente os módulos C em modulos.dll (Windows)
ou modulos.so (Linux/Mac) antes de rodar os testes.
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
    _CMD = ["gcc", "-shared", "-o", _DLL,
            os.path.join(_DIR, "pratos.c"),
            os.path.join(_DIR, "restaurantes.c")]
else:
    _DLL = os.path.join(_DIR, "modulos.so")
    _CMD = ["gcc", "-shared", "-fPIC", "-o", _DLL,
            os.path.join(_DIR, "pratos.c"),
            os.path.join(_DIR, "restaurantes.c")]

_resultado_compilacao = subprocess.run(_CMD, capture_output=True, text=True)
if _resultado_compilacao.returncode != 0:
    raise RuntimeError(
        f"Falha ao compilar módulos C:\n{_resultado_compilacao.stderr}"
    )

# ================================================================
# Estruturas ctypes (espelham exatamente os structs do C)
# ================================================================

class Prato(ctypes.Structure):
    _fields_ = [
        ("IDPrato",    ctypes.c_int),
        ("nome_prato", ctypes.c_char * 100),
        ("descricao",  ctypes.c_char * 300),
        ("CNPJ_rest",  ctypes.c_longlong),
        ("endereco",   ctypes.c_char * 200),
    ]

class ListaPratos(ctypes.Structure):
    _fields_ = [
        ("itens",      ctypes.POINTER(Prato)),
        ("quantidade", ctypes.c_int),
    ]

class Restaurante(ctypes.Structure):
    _fields_ = [
        ("CNPJ",      ctypes.c_longlong),
        ("endereco",  ctypes.c_char * 200),
        ("nome_rest", ctypes.c_char * 150),
    ]

class ListaRest(ctypes.Structure):
    _fields_ = [
        ("itens",      ctypes.POINTER(Restaurante)),
        ("quantidade", ctypes.c_int),
    ]

# ================================================================
# Carregamento da biblioteca e assinaturas das funções
# ================================================================

lib = ctypes.CDLL(_DLL)

lib.initPratos.argtypes       = [ctypes.c_char_p]
lib.initPratos.restype        = None
lib.savePratos.argtypes       = [ctypes.c_char_p]
lib.savePratos.restype        = None
lib.initRestaurantes.argtypes = [ctypes.c_char_p]
lib.initRestaurantes.restype  = None

lib.getPratos.argtypes        = [ctypes.c_int]
lib.getPratos.restype         = ctypes.POINTER(Prato)
lib.getListaPratos.argtypes   = [ctypes.c_char_p]
lib.getListaPratos.restype    = ctypes.POINTER(ListaPratos)
lib.getMenu.argtypes          = [ctypes.c_longlong]
lib.getMenu.restype           = ctypes.POINTER(ListaPratos)
lib.getFeedPratos.argtypes    = []
lib.getFeedPratos.restype     = ctypes.POINTER(ListaPratos)
lib.getListaRest.argtypes     = [ctypes.c_char_p]
lib.getListaRest.restype      = ctypes.POINTER(ListaRest)
lib.getFeedRest.argtypes      = []
lib.getFeedRest.restype       = ctypes.POINTER(ListaRest)
lib.freeListaPratos.argtypes  = [ctypes.POINTER(ListaPratos)]
lib.freeListaPratos.restype   = None
lib.freeListaRest.argtypes    = [ctypes.POINTER(ListaRest)]
lib.freeListaRest.restype     = None

# ================================================================
# Wrappers Python
#
# Convenção de retorno:
#   dict / list[dict]  → dados encontrados
#   None               → parâmetro inválido (nulo/inexistente)
#   {} / []            → sem parâmetro (parâmetro ausente)
# ================================================================

_AUSENTE = object()   # sentinela para "sem parâmetro"


def get_pratos(id_prato=_AUSENTE):
    """Retorna: dict | None | {}"""
    if id_prato is _AUSENTE or id_prato is None:
        return {}
    ptr = lib.getPratos(ctypes.c_int(id_prato))
    if not ptr:
        return None
    p = ptr.contents
    return {
        "IDPrato":    p.IDPrato,
        "nome_prato": p.nome_prato.decode("utf-8"),
        "descricao":  p.descricao.decode("utf-8"),
        "CNPJ_rest":  p.CNPJ_rest,
        "endereco":   p.endereco.decode("utf-8"),
    }


def get_lista_pratos(nome_prato=_AUSENTE):
    """Retorna: list[dict] | None | []"""
    if nome_prato is _AUSENTE:
        return []
    if nome_prato is None:
        return None
    ptr = lib.getListaPratos(nome_prato.encode("utf-8"))
    if not ptr:
        return None
    lp = ptr.contents
    resultado = [
        {
            "IDPrato":    lp.itens[i].IDPrato,
            "nome_prato": lp.itens[i].nome_prato.decode("utf-8"),
        }
        for i in range(lp.quantidade)
    ]
    lib.freeListaPratos(ptr)
    return resultado


def get_menu(cnpj=_AUSENTE):
    """Retorna: list[dict] | None | []"""
    if cnpj is _AUSENTE or cnpj is None:
        return []
    ptr = lib.getMenu(ctypes.c_longlong(cnpj))
    if not ptr:
        return None
    lp = ptr.contents
    resultado = [
        {
            "IDPrato":    lp.itens[i].IDPrato,
            "nome_prato": lp.itens[i].nome_prato.decode("utf-8"),
        }
        for i in range(lp.quantidade)
    ]
    lib.freeListaPratos(ptr)
    return resultado


def get_feed_pratos():
    """Retorna: list[dict] com MAX_FEED pratos | None"""
    ptr = lib.getFeedPratos()
    if not ptr:
        return None
    lp = ptr.contents
    resultado = [
        {
            "IDPrato":    lp.itens[i].IDPrato,
            "nome_prato": lp.itens[i].nome_prato.decode("utf-8"),
        }
        for i in range(lp.quantidade)
    ]
    lib.freeListaPratos(ptr)
    return resultado


def get_lista_rest(nome_rest=_AUSENTE):
    """Retorna: list[dict] | None | []"""
    if nome_rest is _AUSENTE:
        return []
    if nome_rest is None:
        return None
    ptr = lib.getListaRest(nome_rest.encode("utf-8"))
    if not ptr:
        return None
    lr = ptr.contents
    resultado = [
        {
            "CNPJ":      lr.itens[i].CNPJ,
            "endereco":  lr.itens[i].endereco.decode("utf-8"),
            "nome_rest": lr.itens[i].nome_rest.decode("utf-8"),
        }
        for i in range(lr.quantidade)
    ]
    lib.freeListaRest(ptr)
    return resultado


def get_feed_rest():
    """Retorna: list[dict] com MAX_FEED_REST restaurantes | None"""
    ptr = lib.getFeedRest()
    if not ptr:
        return None
    lr = ptr.contents
    resultado = [
        {
            "CNPJ":      lr.itens[i].CNPJ,
            "nome_rest": lr.itens[i].nome_rest.decode("utf-8"),
        }
        for i in range(lr.quantidade)
    ]
    lib.freeListaRest(ptr)
    return resultado


# ================================================================
# Helpers de geração de dados de teste
# ================================================================

def _gerar_pratos(n, cnpj_base=10_000_000_000_000):
    """Gera n pratos com 8 CNPJs distintos em rodízio."""
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
    """Gera n restaurantes com CNPJs sequenciais a partir de cnpj_base."""
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
    # 8 CNPJs distintos: base+0 … base+7, com 2 pratos cada
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
# Testes — Módulo Restaurantes
# ================================================================

class TestGetListaRest:

    def test_c1_nome_existente(self, restaurantes_populados):
        """C1: busca por nome existente retorna lista_rest com os restaurantes."""
        resultado = get_lista_rest("Restaurante")
        assert isinstance(resultado, list)
        assert len(resultado) > 0
        assert all("nome_rest" in r and "CNPJ" in r for r in resultado)

    def test_c2_nome_nulo(self, restaurantes_populados):
        """C2: nome nulo retorna None."""
        resultado = get_lista_rest(None)
        assert resultado is None

    def test_c3_sem_parametro(self, restaurantes_populados):
        """C3: sem parâmetro retorna lista vazia."""
        resultado = get_lista_rest()
        assert resultado == []


class TestGetFeedRest:

    def test_c1_lista_suficiente(self, restaurantes_populados):
        """C1: com 8 restaurantes (> 6), retorna lista com exatamente 6 itens únicos."""
        resultado = get_feed_rest()
        assert isinstance(resultado, list)
        assert len(resultado) == 6
        cnpjs = [r["CNPJ"] for r in resultado]
        assert len(cnpjs) == len(set(cnpjs)), "Restaurantes do feed devem ser únicos"

    def test_c2_sem_restaurantes(self, restaurantes_vazios):
        """C2: sem restaurantes salvos, retorna None."""
        resultado = get_feed_rest()
        assert resultado is None


# ================================================================
# Testes — Módulo Pratos
# ================================================================

class TestGetPratos:

    def test_c1_id_existente(self, pratos_populados):
        """C1: IDPrato existente retorna dicionário completo do prato."""
        resultado = get_pratos(1)
        assert isinstance(resultado, dict)
        assert resultado["IDPrato"] == 1
        assert "nome_prato" in resultado
        assert "descricao" in resultado

    def test_c2_id_inexistente(self, pratos_populados):
        """C2: IDPrato não existente retorna None."""
        resultado = get_pratos(9999)
        assert resultado is None

    def test_c3_sem_parametro(self, pratos_populados):
        """C3: sem parâmetro retorna dicionário vazio."""
        resultado = get_pratos()
        assert resultado == {}


class TestGetListaPratos:

    def test_c1_nome_existente(self, pratos_populados):
        """C1: nome existente retorna lista de pratos com os campos esperados."""
        resultado = get_lista_pratos("Prato")
        assert isinstance(resultado, list)
        assert len(resultado) > 0
        assert all("IDPrato" in p and "nome_prato" in p for p in resultado)

    def test_c2_nome_nulo(self, pratos_populados):
        """C2: nome nulo retorna None."""
        resultado = get_lista_pratos(None)
        assert resultado is None

    def test_c3_sem_parametro(self, pratos_populados):
        """C3: sem parâmetro retorna lista vazia."""
        resultado = get_lista_pratos()
        assert resultado == []


class TestGetMenu:

    def test_c1_cnpj_existente(self, pratos_populados):
        """C1: CNPJ com pratos cadastrados retorna lista de pratos do restaurante."""
        # CNPJ base+1: tem pratos com IDPrato onde (IDPrato % 8 == 1) → 1, 9, 17, 25
        resultado = get_menu(10_000_000_000_001)
        assert isinstance(resultado, list)
        assert len(resultado) > 0
        assert all("IDPrato" in p for p in resultado)

    def test_c2_cnpj_inexistente(self, pratos_populados):
        """C2: CNPJ sem nenhum prato cadastrado retorna None."""
        resultado = get_menu(99_999_999_999_999)
        assert resultado is None

    def test_c3_sem_parametro(self, pratos_populados):
        """C3: sem parâmetro retorna lista vazia."""
        resultado = get_menu()
        assert resultado == []


class TestGetFeedPratos:

    def test_c1_lista_suficiente(self, pratos_populados):
        """C1: com 25 pratos (> 20), retorna lista com exatamente 20 itens únicos."""
        resultado = get_feed_pratos()
        assert isinstance(resultado, list)
        assert len(resultado) == 20
        ids = [p["IDPrato"] for p in resultado]
        assert len(ids) == len(set(ids)), "Pratos do feed devem ser únicos"

    def test_c2_sem_pratos(self, pratos_vazios):
        """C2: sem pratos salvos, retorna None."""
        resultado = get_feed_pratos()
        assert resultado is None
