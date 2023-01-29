#!/usr/bin/env python
# coding: utf-8

# In[1]:


import pathlib
import pandas as pd
import numpy as np


# In[2]:


import seaborn as sns
from matplotlib import pyplot as plt
sns.set_theme(style="whitegrid")

mem_limit = (0, 1024)
time_limit = (1e-1, 1e9)
legend_title = "Method"
query_file_title = "Query Length: {query_length}"
n_query_title = "Number of Queries: {n_queries}"
hue_order = ['FM-Index', 'Suffix-Array', 'Naive Search']



def create_plots(base_name, df, x_axes):
    for filter_, x_axis in zip(x_axes, x_axes[::-1]):
        options = sorted(np.unique(df[filter_]).tolist())
        for option in options:
            mask = df[filter_] == option
            # memory plot
            g = sns.catplot(
                data=df.loc[mask], kind="bar",
                x=x_axis, y="mem_peak_mbytes", hue="method",
                errorbar="sd", palette="dark", alpha=.6, height=6, hue_order=hue_order
            )
            g.despine(left=True)
            if filter_ == 'query_length':
                g.set_axis_labels("Number of Queries", "Peak Memory / MB")
                g.ax.set(title=query_file_title.format(query_length=option), ylim=mem_limit)
                g.legend.set_title("Method")
                g.fig.savefig(f'{base_name}_mem_filter_query_length_{option:04d}.png', dpi=200., bbox_inches='tight')
                plt.close(g.fig)
            else:
                g.set_axis_labels("Query length", "Peak Memory / MB")
                g.ax.set(title=n_query_title.format(n_queries=option), ylim=mem_limit)
                g.legend.set_title(legend_title)
                g.fig.savefig(f'{base_name}_mem_filter_n_queries_{option:04d}.png', dpi=200., bbox_inches='tight')
                plt.close(g.fig)
            # time plot
            g = sns.catplot(
                data=df.loc[mask], kind="bar",
                x=x_axis, y="query_time_ms", hue="method",
                errorbar="sd", palette="dark", alpha=.6, height=6, hue_order=hue_order
            )
            g.despine(left=True)
            if filter_ == 'query_length':
                g.set_axis_labels("Number of Queries", "Duration / ms")
                g.ax.set(yscale="log", title=query_file_title.format(query_length=option), ylim=time_limit)
                g.legend.set_title("Method")
                g.fig.savefig(f'{base_name}_time_filter_query_length_{option:04d}.png', dpi=200., bbox_inches='tight')
                plt.close(g.fig)
            else:
                g.set_axis_labels("Query length", "Duration / ms")
                g.ax.set(yscale="log", title=n_query_title.format(n_queries=option), ylim=time_limit)
                g.legend.set_title(legend_title)
                g.fig.savefig(f'{base_name}_time_filter_n_queries_{option:04d}.png', dpi=200., bbox_inches='tight')
                plt.close(g.fig)
            


# # Assignment 1

# In[3]:


df_a1 = pd.read_csv('experiment_results_assignment1.csv')
method_list = np.unique(df_a1.method).tolist()
df_a1["method_id"] = df_a1.method.apply(lambda f: method_list.index(f))
df_a1["mem_peak_mbytes"] = df_a1.mem_peak_kbytes / 1024.
df_a1["query_length"] = df_a1.query_file.apply(lambda f: int(f.split('_')[-1].split('.')[0]))


# In[4]:


df_a1


# In[5]:


x_axes = ('query_length', 'query_limit')
create_plots('assignment_1_', df_a1, x_axes)


# # Assigment 2

# ## Part 1

# In[6]:


df_a2_p1 = pd.read_csv('experiment_results_assignment2_part1.csv')
method_list = np.unique(df_a2_p1.method).tolist()
df_a2_p1["method_id"] = df_a2_p1.method.apply(lambda f: method_list.index(f))
df_a2_p1["mem_peak_mbytes"] = df_a2_p1.mem_peak_kbytes / 1024.
df_a2_p1["query_length"] = df_a2_p1.query_file.apply(lambda f: int(f.split('_')[-1].split('.')[0]))


# In[7]:


df_a2_p1


# In[8]:


base_name = 'assignment_2_part1_'

option = np.unique(df_a2_p1.query_length)[0]

g = sns.catplot(
    data=df_a2_p1, kind="bar",
    x='query_limit', y="mem_peak_mbytes", hue="method",
    errorbar="sd", palette="dark", alpha=.6, height=6, hue_order=hue_order
)
g.despine(left=True)
g.set_axis_labels("Number of Queries", "Peak Memory / MB")
g.ax.set(title=query_file_title.format(query_length=option), ylim=mem_limit)
g.legend.set_title("Method")
g.fig.savefig(f'{base_name}_mem_filter_query_length_{option:04d}.png', dpi=200., bbox_inches='tight')
plt.close(g.fig)

# time plot
g = sns.catplot(
    data=df_a2_p1, kind="bar",
    x='query_limit', y="query_time_ms", hue="method",
    errorbar="sd", palette="dark", alpha=.6, height=6, hue_order=hue_order
)
g.despine(left=True)
g.set_axis_labels("Number of Queries", "Duration / ms")
g.ax.set(yscale="log", title=query_file_title.format(query_length=option), ylim=time_limit)
g.legend.set_title("Method")
g.fig.savefig(f'{base_name}_time_filter_query_length_{option:04d}.png', dpi=200., bbox_inches='tight')
plt.close(g.fig)


# ## Part 2

# In[9]:


df_a2_p2 = pd.read_csv('experiment_results_assignment2_part2.csv')
method_list = np.unique(df_a2_p2.method).tolist()
df_a2_p2["method_id"] = df_a2_p2.method.apply(lambda f: method_list.index(f))
df_a2_p2["mem_peak_mbytes"] = df_a2_p2.mem_peak_kbytes / 1024.
df_a2_p2["query_length"] = df_a2_p2.query_file.apply(lambda f: int(f.split('_')[-1].split('.')[0]))


# In[10]:


df_a2_p2


# In[11]:


base_name = 'assignment_2_part2_'
error_tile = 'Query Length: {query_length} - Number of Queries: {query_limit}'


query_length = np.unique(df_a2_p2.query_length)[0]
query_limit = np.unique(df_a2_p2.query_limit)[0]

g = sns.catplot(
    data=df_a2_p2, kind="bar",
    x='error_total', y="mem_peak_mbytes", hue="method",
    errorbar="sd", palette="dark", alpha=.6, height=6, hue_order=hue_order
)
g.despine(left=True)
g.set_axis_labels("Number of Queries", "Peak Memory / MB")
g.ax.set(title=query_file_title.format(query_length=query_length, query_limit=query_limit), ylim=(0, 3000))
g.legend.set_title("Method")
g.fig.savefig(f'{base_name}_mem_filter_query_length_{query_length:04d}_query_limit_{query_limit:04d}.png', dpi=200., bbox_inches='tight')
plt.close(g.fig)

# time plot
g = sns.catplot(
    data=df_a2_p2, kind="bar",
    x='error_total', y="query_time_ms", hue="method",
    errorbar="sd", palette="dark", alpha=.6, height=6, hue_order=hue_order
)
g.despine(left=True)
g.set_axis_labels("Number of Queries", "Duration / ms")
g.ax.set(yscale="log", title=query_file_title.format(query_length=query_length, query_limit=query_limit), ylim=time_limit)
g.legend.set_title("Method")
g.fig.savefig(f'{base_name}_time_filter_query_length_{query_length:04d}_query_limit_{query_limit:04d}.png', dpi=200., bbox_inches='tight')
plt.close(g.fig)


# In[ ]:




