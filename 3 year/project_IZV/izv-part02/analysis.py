#!/usr/bin/env python3.11
# coding=utf-8
from io import BytesIO

import requests
from matplotlib import pyplot as plt
import matplotlib.dates as mdates
import pandas as pd
import seaborn as sns
import numpy as np
from zipfile import ZipFile

sns.set()


# muzete pridat libovolnou zakladni knihovnu ci knihovnu predstavenou na prednaskach
# dalsi knihovny pak na dotaz

# Ukol 1: nacteni dat ze ZIP souboru


def construct_df(file_content, headers: list[str], filename: str) -> pd.DataFrame:
    """
    read dataframe from csv file(byte sequence) and add "region" column to it
    :param file_content csv file as byte sequence
    :param headers column names of the dataframe
    :param filename name of the file from the archive
    """
    regions = {
        "PHA": "00", "STC": "01", "JHC": "02", "PLK": "03", "ULK": "04", "HKK": "05", "JHM": "06", "MSK": "07",
        "OLK": "14", "ZLK": "15", "VYS": "16", "PAK": "17", "LBK": "18", "KVK": "19"
    }
    df = pd.read_csv(BytesIO(file_content), sep=";", encoding="cp1250", names=headers,
                     low_memory=False)

    ind = list(regions.values()).index(filename[:-4])
    reg = list(regions.keys())[ind]
    df["region"] = reg
    return df


def load_data(filename: str) -> pd.DataFrame:
    """
    create dataframe from all of the .csv files of an archive
    :param filename path to the archive
    """
    # tyto konstanty nemente, pomuzou vam pri nacitani
    headers = ["p1", "p36", "p37", "p2a", "weekday(p2a)", "p2b", "p6", "p7", "p8", "p9", "p10", "p11", "p12", "p13a",
               "p13b", "p13c", "p14", "p15", "p16", "p17", "p18", "p19", "p20", "p21", "p22", "p23", "p24", "p27",
               "p28", "p34", "p35", "p39", "p44", "p45a", "p47", "p48a", "p49", "p50a", "p50b", "p51", "p52", "p53",
               "p55a", "p57", "p58", "a", "b", "d", "e", "f", "g", "h", "i", "j", "k", "l", "n", "o", "p", "q", "r",
               "s", "t", "p5a"]

    response = requests.get(filename)
    content = BytesIO(response.content)
    redundant_files = ["08.csv", "09.csv", "10.csv", "11.csv", "12.csv", "13.csv", "CHODCI.csv"]
    main_df = []

    # read zip archives
    with ZipFile(content) as archive:
        for sub_arch_name in archive.namelist():
            s = archive.read(sub_arch_name)

            with ZipFile(BytesIO(s)) as subarchive:
                for file in subarchive.namelist():
                    if file not in redundant_files:
                        file_content = subarchive.read(file)
                        main_df.append(construct_df(file_content, headers, file))

    return pd.concat(main_df, ignore_index=True)


# Ukol 2: zpracovani dat


def parse_data(df: pd.DataFrame, verbose: bool = False) -> pd.DataFrame:
    """
    convert some columns to categorical and float datatype
    :param df downloaded dataframe
    :param verbose show dataframe sizes before and after conversion
    """
    new_df = df.copy()

    new_df["date"] = pd.to_datetime(new_df["p2a"])

    orig_size = new_df.memory_usage(deep=True).sum()

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

    new_size = new_df.memory_usage(deep=True).sum()

    orig_size /= 10 ** 6
    new_size /= 10 ** 6
    if verbose:
        print(f"orig_size={orig_size:.1f} MB")
        print(f"new_size={new_size:.1f} MB")

    return new_df


# Ukol 3: počty nehod oidke stavu řidiče


def plot_state(df: pd.DataFrame, fig_location: str = None, show_figure: bool = False):
    """
    plot bar plots depending on crash reason
    :param df dataframe from the parse_data function
    :param fig_location  location where to save created plot
    :param show_figure bool flag draws plot when is True
    """
    plt_df = df.copy()
    driver_crash_reason = ["invalida", "nemoc, úraz apod.", "pod vlivem alkoholu 1 ‰ a více",
                           "pod vlivem alkoholu do 0.99 ‰", "sebevražda", "řidič při jízdě zemřel"]
    p57_num_vals = [7.0, 6.0, 5.0, 4.0, 9.0, 8.0]

    cat_map = {key: val for key, val in zip(p57_num_vals, driver_crash_reason)}
    plt_df["p57_str"] = df["p57"].map(cat_map).astype('category')

    fig, axes = plt.subplots(3, 2, figsize=(14, 9), layout='constrained', sharex=True)
    fig.suptitle("Počet nehod dle stavu řidiče při nedobrém stavu", fontsize=12)

    for ind, reason in enumerate(driver_crash_reason, 0):
        stats = plt_df[plt_df["p57_str"] == reason].groupby(['region']).count()['p1']
        # calculate subplot position
        row = ind // 2
        col = ind % 2

        sns.barplot(ax=axes[row, col], x=stats.index, y=stats.values, hue=stats.index)
        axes[row, col].set_facecolor('lightgray')
        axes[row, col].title.set_text(f"Stav řidiče: {reason}")
        axes[row, col].set_xlabel('Kraj')
        axes[row, 0].set_ylabel('Počet nehod')

    if show_figure:
        plt.show()
    if fig_location is not None:
        plt.savefig(fig_location)


# Ukol4: alkohol v jednotlivých hodinách


def plot_alcohol(df: pd.DataFrame, fig_location: str = None, show_figure: bool = False):
    """
    plot count plots depending on alcohol crash reason
    :param df dataframe from the parse_data function
    :param fig_location  location where to save created plot
    :param show_figure bool flag draws plot when is True
    """
    regions = ["JHM", "MSK", "OLK", "ZLK"]
    plot_df = df.copy()

    plot_df = plot_df[plot_df['region'].isin(regions)]
    plot_df['region'] = pd.Categorical(plot_df['region'], categories=regions)

    plot_df["p2b_h"] = plot_df["p2b"] // 100
    plot_df = plot_df[plot_df["p2b_h"] < 25]

    plot_df["p2b_h"] = plot_df["p2b_h"].astype("category")
    plot_df["Alkohol"] = (plot_df["p11"].astype(int) >= 3).replace({True: "Ano", False: "Ne"})

    g = sns.catplot(data=plot_df, x="p2b_h", hue="Alkohol", col="region",
                    kind="count", col_wrap=2, hue_order=["Ano", "Ne"],
                    legend=False, sharey=False
                    )

    # set titles for each subplot
    g.set_titles("Kraj: {col_name}")
    # move the legend outside the subplots
    g.fig.legend(labels=["Ano", "Ne"], title="Alkohol", bbox_to_anchor=(1.1, 0.5), loc='center right', ncol=1)
    g.set_axis_labels("Hodina", "Počet nehod")

    if show_figure:
        plt.show()
    if fig_location is not None:
        g.savefig(fig_location)


# Ukol 5: Zavinění nehody v čase


def plot_fault(df: pd.DataFrame, fig_location: str = None, show_figure: bool = False):
    """
    plot count plots depending on guilty of the crash, i.e. who is responsible for the crash
    :param df dataframe from the parse_data function
    :param fig_location  location where to save created plot
    :param show_figure bool flag draws plot when is True
    """
    regions = ['JHM', 'MSK', 'OLK', 'ZLK']
    zavineni = [1, 2, 3, 4]
    plot_df = df.copy()

    plot_df = plot_df[plot_df['region'].isin(regions)]
    plot_df = plot_df[plot_df['p10'].isin(zavineni)]

    # adjust categorical data to restrict them only on selected categories
    plot_df['region'] = pd.Categorical(plot_df['region'], categories=regions)
    plot_df['p10'] = pd.Categorical(plot_df['p10'], categories=zavineni)
    plot_df['p10'] = plot_df['p10'].replace(
        {1: "Řidičem motorového vozidla", 2: "Řidičem nemotorového vozidla", 3: "Chodcem", 4: "Zvířetem"})

    # create pivot table
    pivot_table = pd.pivot_table(plot_df, values='p1', index=['region', 'date'], columns='p10', aggfunc='count',
                                 fill_value=0)

    # sub-sample at the monthly level
    monthly_sample = pivot_table.groupby(['region', pd.Grouper(freq='M', level='date')]).sum()

    # convert to stacked format and select date span
    stacked_df = monthly_sample.stack(level='p10').reset_index()
    stacked_df = stacked_df.rename(columns={0: 'count'})
    stacked_df = stacked_df[(stacked_df['date'] >= '2016-01-01') & (stacked_df['date'] < '2023-01-01')]

    g = sns.FacetGrid(stacked_df, col="region", height=5, aspect=1.4, col_wrap=2, sharey=True, sharex=False,
                      legend_out=True)
    # map line plots onto the grid
    g.map_dataframe(
        sns.lineplot,
        x='date',
        y='count',
        hue='p10'
    )
    # set legend for the whole plot
    g.add_legend(title="Zavínění", bbox_to_anchor=(1.05, 0.5))

    # set xticks and labels for subplots
    g.set_axis_labels(x_var='', y_var='Počet nehod')

    g.set_titles(col_template="Kraj: {col_name}")

    for ax in g.axes.flat:
        ax.xaxis.set_major_formatter(mdates.DateFormatter('%m/%y'))

    g.set(xlim=(pd.Timestamp('2016-01-01'), pd.Timestamp('2023-01-01')))
    plt.subplots_adjust(wspace=0.2)

    if show_figure:
        plt.show()
    if fig_location is not None:
        g.savefig(fig_location)


if __name__ == "__main__":
    # zde je ukazka pouziti, tuto cast muzete modifikovat podle libosti
    # skript nebude pri testovani pousten primo, ale budou volany konkreni
    # funkce.
    df = load_data("http://ehw.fit.vutbr.cz/izv/data.zip")
    df2 = parse_data(df, True)
    plot_state(df2, "01_state.png")
    plot_alcohol(df2, "02_alcohol.png")
    plot_fault(df2, "03_fault.png")

# Poznamka:
# pro to, abyste se vyhnuli castemu nacitani muzete vyuzit napr
# VS Code a oznaceni jako bunky (radek #%%% )
# Pak muzete data jednou nacist a dale ladit jednotlive funkce
# Pripadne si muzete vysledny dataframe ulozit nekam na disk (pro ladici
# ucely) a nacitat jej naparsovany z disku
