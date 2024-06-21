#!/usr/bin/python3.10
# coding=utf-8
import pandas as pd
import geopandas
import matplotlib.pyplot as plt
import contextily
import sklearn.cluster
import numpy as np


# muzete pridat vlastni knihovny

def format_data(df: pd.DataFrame) -> pd.DataFrame:
    new_df = df.copy()

    new_df["p2a"] = pd.to_datetime(new_df["p2a"])
    new_df.loc[new_df['d'] == "D:", ['d', 'e']] = np.nan
    new_df.drop_duplicates(subset=['p1'], inplace=True)

    not_category = ["p1", "p2a", "p2b", "p13a", "p13b", "p13c", "p14", "p34", "p37", "p53", "date"]

    for col in new_df.columns:
        if col not in not_category:
            new_df[col] = new_df[col].astype('category')

    float_cols = ["p37", "a", "b", "d", "e", "f", "g", "o", "n", "r", "s"]
    for col in float_cols:
        new_df[col] = new_df[col].astype(str).str.replace(',', '.')
        new_df[col] = pd.to_numeric(new_df[col], errors='coerce')
        new_df[col] = new_df[col].astype(float)

    return new_df


def make_geo(df: pd.DataFrame) -> geopandas.GeoDataFrame:
    """ Konvertovani dataframe do geopandas.GeoDataFrame se spravnym kodovani"""
    pass


def plot_geo(gdf: geopandas.GeoDataFrame, fig_location: str = None,
             show_figure: bool = False):
    """ Vykresleni grafu s nehodami  """
    pass


def plot_cluster(gdf: geopandas.GeoDataFrame, fig_location: str = None,
                 show_figure: bool = False):
    """ Vykresleni grafu s lokalitou vsech nehod v kraji shlukovanych do clusteru """
    pass


if __name__ == "__main__":
    # zde muzete delat libovolne modifikace
    gdf = make_geo(pd.read_pickle("accidents.pkl.gz"))
    plot_geo(gdf, "geo1.png", True)
    plot_cluster(gdf, "geo2.png", True)
