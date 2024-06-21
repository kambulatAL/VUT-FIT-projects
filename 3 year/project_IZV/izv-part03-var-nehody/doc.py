import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import matplotlib

matplotlib.use('TkAgg')


# Goal: To show on which types of solid obstacles accidents injure people more often than others

def read_df(path: str) -> pd.DataFrame:
    """
    Read dataframe from the archive/file
    :param path path to the archive/file
    """
    return pd.read_pickle(path)


'''
p8 abbreviations:
    1 - strom
    2 - sloup
    3 - odrazník, patník
    4 - svodidlo
    5 - překážka vzniklá provozem jiného vozidla
    6 - zeď, pevná část mostů
    7 - závory železničního přejezdu
    8 - překážka vzniklá stavební činností
    9 - jiná překážka
    0 - nepříchází v úvahu (this type won't be included as it's not a hard obstacle)

p9 abbreviations:
    1 - nehoda s následky na životě
    2 - nehoda pouze s hmotnou škodou
'''


def reformat_df(df: pd.DataFrame) -> pd.DataFrame:
    """
    Select from the original dataframe values grouped by the p8 and p9 columns,
    excluding values with "0" in column p8, because we need only hard obstacle categories.
    :param df the original dataframe from a file/archive
    """
    new_df = df.query("p8 != 0")
    new_df = new_df.groupby(["p8", "p9"]).size().to_frame(name='count').reset_index()
    new_df = new_df.rename(columns={"p8": "type of hard obstacle", "p9": "type of accident"})
    return new_df


def rename_df_values(df: pd.DataFrame) -> pd.DataFrame:
    """
    Replace numeric representations of the categories with the textual analogs
    :param df dataframes got from the reformat_df function
    """
    p8_labels = {
        1: "strom", 2: "sloup", 3: "odrazník, patník", 4: "svodidlo",
        5: "provoz jiného vozidla", 6: "zeď, pevná část mostů",
        7: "závory železničního přejezdu", 8: "stavební činnost",
        9: "jiná překážka"
    }
    p9_labels = {1: "nehoda s následky na životě", 2: "nehoda pouze s hmotnou škodou"}

    return df.replace({"type of hard obstacle": p8_labels, "type of accident": p9_labels})


def plot_graph(df: pd.DataFrame, fig_location: str = None, show_figure: bool = False):
    """
    plot bar plots depending on the type of hard obstacle
    :param df dataframe from the rename_df_values function
    :param fig_location  location where to save created plot
    :param show_figure bool flag draws plot when is True
    """
    sns.set(style="whitegrid")
    plt.figure(figsize=(12, 8))
    sns.barplot(data=df, x='type of hard obstacle', y='count', hue='type of accident', dodge=True)
    plt.xticks(rotation=45, ha='right')
    plt.tight_layout()

    if fig_location:
        plt.savefig(fig_location)
    if show_figure:
        plt.show()


def create_tex_table(df: pd.DataFrame):
    """
    Create table with data of our interests and convert it to the latex format
    :param df dataframe from the rename_df_values function
    """
    table = pd.pivot_table(df, index="type of hard obstacle", columns="type of accident", values="count")
    tex = table.to_latex(
        caption="Type of accident depending on the type of hard obstacle over the whole time of observations",
        position="h",
        bold_rows=True)

    # Print the latex table
    print("////// INSERT TABLE //////")
    print(tex)
    print("////// INSERT TABLE ////// \n\n\n")


if __name__ == "__main__":
    df = read_df("accidents.pkl.gz")
    new_df = rename_df_values(reformat_df(df))
    plot_graph(new_df, fig_location="fig.png")
    create_tex_table(new_df)

    # Printing the results

    # the whole number of accidents for the all types of hard obstacle
    accidents = new_df['count'].sum()

    # all cases of injuries from a collision with the one of types of hard obstacle
    injured_cases = new_df[new_df['type of accident'] == 'nehoda s následky na životě']
    hurt_cnt = injured_cases['count'].sum()

    print(f"Count of  all cases of injuries from a collision with the one of types of hard obstacle: {hurt_cnt}")

    proc_inj = hurt_cnt / accidents * 100
    print(f'Percentage of all accidents with consequences to life caused by one '
          f'of the types of hard obstacle in relation to all accidents: {proc_inj:.2f} %')

    # count of the cases of injuries from a collision with trees
    tree_injuries = injured_cases[injured_cases["type of hard obstacle"] == 'strom']['count'][0]
    proc_tree_inj = tree_injuries / hurt_cnt * 100

    print(f'Percentage of tree crash injuries in relation to all cases of '
          f'injured persons: {proc_tree_inj:.2f} %')

    # count of the cases of injuries from a collision with another vehicle
    car_injuries = injured_cases[injured_cases["type of hard obstacle"] == 'provoz jiného vozidla']['count'][8]
    proc_car_injuries = car_injuries / hurt_cnt * 100

    print('Percentage of another vehicle crash  injuries in relation to all cases of '
          f'injured persons: {proc_car_injuries:.2f} %')
