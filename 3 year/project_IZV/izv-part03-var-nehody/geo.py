#!/usr/bin/python3.10
# coding=utf-8
import matplotlib
import pandas as pd
import geopandas
import matplotlib.pyplot as plt
import contextily as ctx
import sklearn.cluster as clstr
import numpy as np
from shapely.geometry import Point, box
from matplotlib import cm
from matplotlib.colors import Normalize

matplotlib.use('TkAgg')


# muzete pridat vlastni knihovny

def reformat_data(df: pd.DataFrame) -> pd.DataFrame:
    """
    Read dataframe, drop duplicates and NaN values
    :param df dataframe
    """
    new_df = df.copy()

    new_df["p2a"] = pd.to_datetime(new_df["p2a"])
    new_df.drop_duplicates(subset=['p1'], inplace=True)
    new_df.dropna(subset=['e', 'd'], inplace=True)
    new_df = new_df.dropna(axis=1)
    return new_df


def make_geo(df: pd.DataFrame) -> geopandas.GeoDataFrame:
    """
    Convert dataframe to geopandas.GeoDataFrame with a proper coordinates encoding
    :param df dataframe 
    """
    # reformat data and select region
    new_df = reformat_data(df).query('region == "JHM"')
    # create points with the given coordinates
    geometry = [Point(xy) for xy in zip(new_df['d'], new_df['e'])]

    # Create a GeoDataFrame
    gdf = geopandas.GeoDataFrame(new_df, geometry=geometry, crs='EPSG:5514')

    return gdf


def plot_geo(gdf: geopandas.GeoDataFrame, fig_location: str = None,
             show_figure: bool = False):
    """
        Plot a graph with the defined conditions from the task
        :param gdf GepDataFrame
        :param fig_location  location where to save created plot
        :param show_figure bool flag draws plot when is True
        """

    # create a bounding box to contraint drawing and points
    bounds = [-935719.38, -1244158.89, -418597.43, -911041.21]
    bounding_box = box(*bounds)

    # filter points within the bounding box
    filtered = gdf[gdf.geometry.within(bounding_box)]

    condition_2021 = (filtered['p10'] == 4) & (filtered['date'].dt.year == 2021)
    condition_2022 = (filtered['p10'] == 4) & (filtered['date'].dt.year == 2022)

    fig, ax = plt.subplots(1, 2, figsize=(15, 12))
    plt.tight_layout()

    # plot for 2021
    filtered[condition_2021].plot(ax=ax[0], color='blue', markersize=5, alpha=0.6)
    ax[0].set_title('Nehody zaviněné zvěří v JHM kraji roce 2021')

    # plot for 2022
    filtered[condition_2022].plot(ax=ax[1], color='blue', markersize=5, alpha=0.6)
    ax[1].set_title('Nehody zaviněné zvěří v JHM kraji roce 2022')

    for axis in ax:
        ctx.add_basemap(axis, crs=gdf.crs.to_string(), source=ctx.providers.OpenStreetMap.Mapnik, alpha=0.9, zoom=10)
        axis.set_axis_off()

    # Save or show the figure
    if fig_location:
        plt.savefig(fig_location)
    if show_figure:
        plt.show()


def plot_cluster(gdf: geopandas.GeoDataFrame, fig_location: str = None,
                 show_figure: bool = False):
    """
        Plot a graph with the all road accidents in the selected region as clusters
        :param gdf GepDataFrame 
        :param fig_location  location where to save created plot
        :param show_figure bool flag draws plot when is True
        """

    # create a bounding box to contraint drawing and points
    bounds = [-935719.38, -1244158.89, -418597.43, -911041.21]
    bounding_box = box(*bounds)

    # filter points within the bounding box
    filtered = gdf[gdf.geometry.within(bounding_box)]
    filtered = filtered[filtered['p11'] >= 4]

    # clusterize data using k-means ML algorithm
    filtered['cluster_labels'] = clstr.KMeans(n_clusters=12, random_state=42, n_init="auto").fit_predict(
        filtered[['d', 'e']])

    # calculate convex hulls for each cluster
    convex_hulls = filtered.groupby('cluster_labels')['geometry'].apply(
        lambda x: x.unary_union.convex_hull).reset_index()

    # create polygons for emphasizing clusters' areas with color
    polygons = geopandas.GeoDataFrame(geometry=convex_hulls['geometry'], crs=gdf.crs)

    fig, ax = plt.subplots(figsize=(13, 13))

    # plot polygons based on convex hulls
    polygons.plot(ax=ax, color='grey', alpha=0.3)

    filtered.plot(ax=ax, column='cluster_labels', cmap='plasma', markersize=5)

    # create a separate ScalarMappable for the colorbar
    cluster_counts = filtered['cluster_labels'].value_counts()

    norm = Normalize(vmin=cluster_counts.min(), vmax=cluster_counts.max())
    cmap = cm.ScalarMappable(norm=norm, cmap='plasma')
    cmap.set_array([])

    # create a colorbar based on the ScalarMappable
    cbar = plt.colorbar(cmap, ax=ax, orientation='horizontal', pad=0.02, label='Počet nehod v úseku')

    # add basemap to show a terrain
    ctx.add_basemap(ax, crs=gdf.crs.to_string(), source=ctx.providers.OpenStreetMap.Mapnik, alpha=0.9, zoom=10)
    ax.set_axis_off()
    ax.set_title('Nehody v JHM kraji s významnou měrou alkoholu')

    # save or show the figure
    if fig_location:
        plt.savefig(fig_location)
    if show_figure:
        plt.show()


if __name__ == "__main__":
    # zde muzete delat libovolne modifikace
    gdf = make_geo(pd.read_pickle("accidents.pkl.gz"))
    plot_geo(gdf, "geo1.png", False)
    plot_cluster(gdf, "geo2.png", False)
