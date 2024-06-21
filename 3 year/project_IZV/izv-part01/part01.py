#!/usr/bin/env python3
"""
IZV cast1 projektu
Autor: Kambulat Alakaev(xalaka00)
"""

import matplotlib
from bs4 import BeautifulSoup
import requests
import numpy as np
from numpy.typing import NDArray
import matplotlib.pyplot as plt
from typing import List, Callable, Dict, Any

# set Tkinter backend for Matplotlib
matplotlib.use('TkAgg')


def integrate(f: Callable[[NDArray], NDArray], a: float, b: float, steps=1000) -> float:
    # create an array of points
    arr = np.linspace(a, b, steps)
    # calculate part of the integral(x_i - x_i-1) analytically
    arr1 = arr[1:] - arr[:-1]
    # calculate arguments for the 'f' func ((x_i + x_i-1)/2) analytically
    arr2 = (arr[1:] + arr[:-1]) / 2
    # multiply 2 calculated np arrays
    res = arr1 * f(arr2)
    return float(np.sum(res))


def generate_graph(a: List[float], show_figure: bool = False, save_path: str = None):
    # create an array of points
    span = np.linspace(-3, 3, 10000)
    np_a = np.array(a) ** 2
    # calculate the formula
    res = np.array(np_a[:, np.newaxis] * span ** 3 * np.sin(span)).reshape((3, -1))

    colors = ['blue', 'orange', 'lightgreen']
    a_vals = [1.0, 1.5, 2.0]
    integrals = [np.trapz(res[i], span) for i in range(3)]

    # plot each graph in a cycle
    fig, ax = plt.subplots()
    for i, color, a_val in zip(range(3), colors, a_vals):
        ax.plot(span, res[i], label=r'$\gamma_{{{}}}(X)$'.format(a_val))
        ax.fill_between(span, res[i], color=color, zorder=2, alpha=0.2)
        ax.annotate(r'$\int f_{{{a_val}}}(x)dx = {val:.2f}$'.format(a_val=a_vals[i], val=integrals[i]),
                    xy=(span[-1], res[i, -1]))

    # set labels, tick range and legend
    ax.set_xlabel('x')
    ax.set_ylabel(r'$f_a(X)$')
    ax.legend(loc='center', bbox_to_anchor=(0.5, 1.05), ncol=3)

    ax.set_xlim(-3, 6)
    ax.set_ylim(0, 40)
    ax.set_xticks(range(-3, 4))

    if show_figure:
        plt.show()
    if save_path is not None:
        plt.savefig(save_path)


def generate_sinus(show_figure: bool = False, save_path: str = None):
    # create an array of points
    span = np.linspace(0, 100, 10000)
    # calculate y values for graphs
    f1 = np.array(0.5 * np.cos(1 / 50 * span * np.pi))
    f2 = np.array(0.25 * (np.sin(np.pi * span) + np.sin(3 / 2 * np.pi * span)))
    f_12 = f1 + f2
    y_values = [f1, f2]
    y_ticks = np.linspace(-0.8, 0.8, 5)
    y_labels = [r'$f_1(t)$', r'$f_2(t)$', r'$f_1(t) + f_2(t)$']

    fig, axes = plt.subplots(3, 1, figsize=(10, 6), layout='constrained')
    for i in [0, 1, 2]:
        if i == 2:
            mask = f_12 > f1
            axes[2].plot(span, f_12, color='green')
            f_12[mask] = np.nan
            axes[2].plot(span, f_12, color='red')
        else:
            axes[i].plot(span, y_values[i])

        axes[i].set_ylim(-0.8, 0.8)
        axes[i].set_xlim(0, 100)
        axes[i].set_yticks(y_ticks)
        axes[i].set_xlabel('t')
        axes[i].set_ylabel(y_labels[i])

    if show_figure:
        plt.show()
    if save_path is not None:
        plt.savefig(save_path)


def reformat_data(d: dict):
    """
    convert obtained data from a website to the readable format
    :param d - record with data
    """
    d['lat'] = d['lat'][:-1]
    d['long'] = d['long'][:-1]
    for i in ['lat', 'long', 'height']:
        d[i] = float(d[i].replace(',', '.'))
    return d


def get_data(html):
    """
    parse given html page and get the target data
    :param html - html page
    """
    soup = BeautifulSoup(markup=html, features='html.parser', from_encoding='utf-8')

    table = soup.find_all('table')[1]
    rows = table.find_all('tr')[1:]
    keys = ['position', 'lat', 'long', 'height']
    res = []

    for row in rows:
        arr = np.array([], dtype=object)
        for cell in row.find_all('td'):
            arr = np.append(arr, cell.text)

        arr = list(arr[[0, 2, 4, 6]])
        d = {key: val.replace('\xa0\xa0', '') for key, val in zip(keys, arr)}
        res.append(reformat_data(d))
    return res


def download_data() -> List[Dict[str, Any]]:
    path_to_page = 'https://ehw.fit.vutbr.cz/izv/st_zemepis_cz.html'
    response = requests.get(path_to_page)
    return get_data(response.content)


