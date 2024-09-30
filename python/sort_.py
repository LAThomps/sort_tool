#!/usr/bin/python3

import argparse
import re
import sys
from copy import deepcopy as dcopy
from itertools import chain
from typing import Literal

def preprocess(
        file_name, 
        unique: bool = False, 
        add_rand_hash: bool = False,
        radix: bool = False
    ):
    """
    Processing in the .txt file

    Parameters
    ----------
    file_name : str, file_path like
        .txt file to be ingested
    unique : bool, optional
        Based on -u flag arg, by default False
    add_rand_hash : bool, optional
        Based on the -R flag arg, by default False
    radix : bool, optional
        Based on the sort_type arg, by default False

    Returns
    -------
    list, list, dict, int
        Four outputs:
            words/words_set: list
                list of individual words to be sorted, can be unique or
                non-unique based on unique arg
            idx_to_sort: list
                only used for -R sorting, sorts the indicies then maps 
                back to the words at the end
            idx_word_map: dict
                only used for -R sorting, dictionary to map index list 
                back to the words for output.
             max_len: int
                longest word in the words list, used for the radix sort
    """
    # make placeholder vars (last two only used for -R sorting)
    words = []
    max_len = 0
    idx_to_sort = []
    idx_word_map = {}

    # read in .txt file
    with open(file_name, 'r', encoding='utf-8-sig') as file:
        # use regex to find valid word strings, add the line if not empty
        for line in file:
            add_line = re.findall(r"[A-Za-z]+", line)
            if add_line:
                word_lengths = [len(word) for word in add_line]
                # keep running track of max_len
                max_len = max(max_len, max(word_lengths))
                [words.append(word) for word in add_line]
    
    # sorting output based on input args
    if add_rand_hash or unique:
        words_set = list(set(words))
        # make mapping index list to be sorted for -R sorting
        if add_rand_hash:
            if radix:
                hash_idx = [f"{i}" for i in range(len(words_set))]
            else:
                hash_idx = [i for i in range(len(words_set))]
            # word_idx_map for converting words to indicies to sort
            word_idx_map = {word:i for word, i in zip(words_set, hash_idx)}
            # idx_word_map to map those indicies back to words
            idx_word_map = {i:word for i, word in zip(hash_idx, words_set)}
            # list that will actually be sorted during -R sorting
            idx_to_sort = list(map(word_idx_map.get, words))
            words = words_set if unique else words
            return words, idx_to_sort, idx_word_map, max_len
        # for just unique sorts
        else:
            return words_set, idx_to_sort, idx_word_map, max_len
    # non-unique and non -R sorting
    else:
        return words, idx_to_sort, idx_word_map, max_len
    

def quick_sort(words):
    """
    Standard quick sort algorithm

    Parameters
    ----------
    words : list
        list of words to be sorted

    Returns
    -------
    list
        sorted list
    """
    # base case
    if len(words) <= 1:
        return words 
    else:
        # choose half way pivot
        pivot_position = len(words) // 2
        pivot = words[pivot_position]

        # sort based on values related to pivot
        left = [w for w in words if w < pivot]
        middle = [w for w in words if w == pivot]
        right = [w for w in words if w > pivot]

        # sort partitioned lists until base case
        return quick_sort(left) + middle + quick_sort(right)

def merge_sort(words):
    """
    Merge sort algorithm

    Parameters
    ----------
    words : list
        list of words to be sorted

    Returns
    -------
    list
        sorted list
    """
    # base case
    if len(words) <= 1:
        return words 
    else:
        # break list in two until all element in list of len == 1
        mid = len(words) // 2
        left = merge_sort(words[:mid])
        right = merge_sort(words[mid:])

        # merge partitioned lists to sort
        return merge(left, right)

def merge(left, right):
    """
    Merge sort sub routine

    Parameters
    ----------
    left : list
        left partition from merge function
    right : list   
        right partition from merge function

    Returns
    -------
    list   
        sorted list
    """
    output = []
    i = j = 0

    # while still have elements to compare in both lists
    while i < len(left) and j < len(right):
        # append to output stack based on vals
        if left[i] < right[j]:
            output.append(left[i])
            i += 1
        else:
            output.append(right[j])
            j += 1

    # put the rest of the values of the list we didn't get through
    # at the top of the stack (this is fine because these elements
    # are all greater than the last element added)
    output += left[i:]
    output += right[j:]

    return output

def heap_sort(words):
    """
    Heap sort algorithm

    Parameters
    ----------
    words : list
        list to be sorted

    Returns
    ----------
    None: sorts the list in place
    """
    n = len(words)

    # build max heap
    for i in range(n // 2 - 1, -1, -1):
        heapify(words, n, i)
    
    # take out inidividual elements from heap
    for i in range(n - 1, 0, -1):
        words[i], words[0] = words[0], words[i]
        heapify(words, i, 0)

def heapify(words, n, i):
    """
    Heap sort sub routine

    Parameters
    ----------
    words : list
        list to be heapified
    n : int
        length of heap to modify
    i : int
        index of initial root of heap
    """
    # big is root of heap, left/right are children indicies
    big = i
    left = 2 * i + 1
    right = 2 * i + 2

    # update if children larger than root
    if left < n and words[left] > words[big]:
        big = left 
    
    # check both children
    if right < n and words[right] > words[big]:
        big = right 

    # orig root not largest
    if big != i:
        # swap these vals
        words[i], words[big] = words[big], words[i]
        # heapify subtree
        heapify(words, n, big)


def radix_sort(words, longest_word):
    """
    Radix sort algorithm

    Parameters
    ----------
    words : list
        list to be sorted
    longest_word : int
        length of longest word in the list

    Returns
    -------
    list
        sorted list
    """
    # these are non-letters in the middle of the chr range from 'A' to 'z'
    bad_set = {i for i in range(91,97)}
    # generates a string from 'A' to 'z' in lexicographical order
    order = "".join([chr(i) for i in range(65,123) if i not in bad_set])
    # base empty table to store the stacks/queues in the sort
    std_radix = [[] for char in order]
    max_iter = longest_word
    # sets which index letter we're evaluating for every word
    level = max_iter

    # do until first characters are encountered
    while level > 0:
        # make a new table to move values to
        new_radix = dcopy(std_radix)

        # for first time through
        if level == max_iter:
            for word in words:
                # count shorter words as being right padded with the 'A' 
                # character, similar to zero padding integers
                if len(word) < max_iter:
                    new_radix[0].append(word)
                # move the word to the appropriate bin
                else:
                    letter = word[-1]
                    idx = map_val(letter)
                    # add to stack in that bin
                    new_radix[idx].append(word)
            # for use in next iteration
            old_radix = dcopy(new_radix)
        # for all other iterations
        else:
            # for each character bin
            for j in range(len(order)):
                # if the bin is not empty
                while old_radix[j]:
                    # treat old stack as queue now
                    temp = old_radix[j].pop(0)
                    # same 'A' padding logic used in first step
                    if len(temp) < level:
                        idx = 0
                    # if word has valid letters at this length, calculcate
                    # where that word should go next
                    else:
                        letter = temp[level - 1]
                        idx = map_val(letter)
                    # move word to next appropriate bin stack
                    new_radix[idx].append(temp)
            # store for next use
            old_radix = dcopy(new_radix)
        level -= 1
    # just output standard list
    return list(chain(*new_radix))


def map_val(letter):
    """
    Mapping alphabetical string to ord value

    Parameters
    ----------
    letter : str
        ascii letter

    Returns
    -------
    int
        index in string of alphabetical characters used in radix sort
    """
    num_val = ord(letter)
    if num_val < 97:
        return num_val - 65
    else:
        return num_val - 71

def sort(
        words, 
        sort_type: Literal['radix','merge','quick','heap'], 
        max_len
    ):
    """
    General sort director

    Parameters
    ----------
    words : list
        list to be sorted
    sort_type : Literal[radix, merge, quick, heap]
        type of sort to conduct
    max_len : int
        length of longest word, used only for radix sort

    Returns
    -------
    list
        sorted list
    """
    if sort_type == "quick":
        return quick_sort(words)
    elif sort_type == "merge":
        return merge_sort(words)
    elif sort_type == "heap":
        heap_sort(words)
        return words
    else:
        return radix_sort(words, max_len)
    

def create_parser():
    """
    Command line argument parser

    Returns
    -------
    argparse.ArgumentParser
        Namespace for command line argumetns
    """
    parser = argparse.ArgumentParser(description="Sort App")
    parser.add_argument("filename", help=".txt file to be sorted")
    parser.add_argument(
        "sort_type",
        choices=["radix", "merge", "quick", "heap"],
        default="quick",
        help="sort method"
    )
    parser.add_argument(
        "-u",
        "--uniq",
        help="sort unique words", 
        action="store_true"
    )
    parser.add_argument(
        '-d',
        '--desc',
        help="sort in descending order",
        action="store_true"
    )
    parser.add_argument(
        "-R",
        "-random-sort",
        help="sort by a random order",
        action="store_true"
    )
    return parser


def main():
    """
    Main function for script
    """
    # read in arguments
    parser = create_parser()
    args = parser.parse_args()

    # ingest text file, create helper variables based on args
    ingest = preprocess(
        args.filename, 
        unique=args.uniq,
        add_rand_hash=args.R,
        radix=True if args.sort_type=="radix" else False 
    )
    # unpack output from preprocess function
    words, idx_to_sort, idx_word_map, max_len = [i for i in ingest]

    # random sort
    if args.R:
        sorted_idx = sort(idx_to_sort, args.sort_type, max_len)
        sorted_words = list(map(idx_word_map.get, sorted_idx))
    # sort normal
    else:
        sorted_words = sort(words, args.sort_type, max_len)
    # reverse the sort if they ask for it
    if args.desc:
        sorted_words.reverse()
    # output all unless piped into head function
    for word in sorted_words:
        try: 
            print(word)
        except BrokenPipeError:
            sys.exit(0)

if __name__ == '__main__':
    main()
