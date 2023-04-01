import uproot
import uproot3
import numpy as np


class load_tree():

    def __init__(self):
        super().__init__()
        

    def convert_to_grid(self, key1, all_ttrees, return_with_keys = True, use_vars=0):
        count = 0
        val_arr = 0
        key_arr = 0
        for key2 in all_ttrees[key1].keys():
            if use_vars==0 or (key2.decode() in use_vars):
                if count==0:
                    key_arr = np.array([key2])
                    val_arr = np.array([all_ttrees[key1][key2].array()])
                else:
                    key_arr = np.append(key_arr, key2)
                    val_arr = np.append(val_arr, all_ttrees[key1][key2].array())
                    val_arr = np.reshape(val_arr, (count+1,-1))
                count+=1      
        if return_with_keys:
            return key_arr.astype(str), list(val_arr.T)
        else:
            return list(val_arr.T)

    def print_trees(self, path):
        Tree = uproot3.open(path)
        all_ttrees= dict(Tree.allitems(filterclass=lambda cls:issubclass(cls, uproot3.tree.TTreeMethods)))
        print(all_ttrees)
        return Tree

    def load(self, path, limit=np.infty, use_vars=0, key=False, save=False, save_path=[], verbose=False):

        Tree = uproot3.open(path)
        all_ttrees= dict(Tree.allitems(filterclass=lambda cls:issubclass(cls, uproot3.tree.TTreeMethods)))

        if key:
            out_keys, out_data = self.convert_to_grid(key, all_ttrees, return_with_keys = True, use_vars=use_vars)
        else:
            out_keys = 0
            out_data = 0
            counter = 0
            for key_idx in all_ttrees.keys():
                
                if verbose:
                    print("Loading directory:", key_idx)

                if counter>limit:
                    break
                else:
                    if counter == 0:
                        out_keys, out_data = self.convert_to_grid(key_idx, all_ttrees, return_with_keys = True, use_vars=use_vars)
                    else:
                        out_data.extend(self.convert_to_grid(key_idx, all_ttrees, return_with_keys = False, use_vars=use_vars))
                        if save:
                            np.savetxt(save_path[counter], out_data, fmt="%s")
                counter+=1

        return out_keys, np.array(out_data).astype(float)

    def export_to_tree(self, path, labels, data):

        # file = uproot.recreate(filename,initial_streamers_bytes=20000)
        file = uproot.recreate(path)
        writing_data = data.T
        dicts = {}

        for i, key in enumerate(labels.tolist()):
            dicts[key] = writing_data[i]

        file['data_tree'] = dicts
        file.close()

        print("Successfully exported data to TTree!\n")