## 从楼层扔下鸡蛋的问题

### 题目：给定K个鸡蛋，鸡蛋只会在某个特定的层高及以上才会摔碎，问最坏情况要找出这个临界层高需要测试多少次？

### 分析：
设`dp[M][K]`表示`K`个鸡蛋经过`M`次测试最多可以确认的楼层高度。

那么就有：
```c++
dp[m][k] = dp[m-1][k-1]+dp[m-1][k]+1
```
其含义是，我们用一个鸡蛋测试某个楼层花费了1次；若鸡蛋破碎，则还能测试`dp[m-1][k-1]`的楼层高度；
若鸡蛋不破碎，则还能测试`dp[m-1][k]`的楼层高度；

`dp[m][k]` 会以指数方式增长到`N`。

因此时间复杂度为： `O(KlogN)` 空间复杂度为：`O(NK)`

```c++
int superEggDrop(int K, int N) {
    vector<vector<int>> dp(N + 1,vector<int>(K + 1, 0));
    int m = 0;
    while (dp[m][K] < N) {
        m++;
        for (int k = 1; k <= K; ++k)
            dp[m][k] = dp[m - 1][k - 1] + dp[m - 1][k] + 1;
    }
    return m;
}
```

优化为一维数组

```c++
int superEggDrop(int K, int N) {
  vector<int> dp(K + 1, 0);
  int m;
  for(m = 0; dp[K] < N; m++) {
    for(int k = K; k > 0; k--) {
      dp[k] += dp[k-1] + 1;
    }
  }
  return m;
}
```

时间复杂度为： `O(KlogN)` 空间复杂度为：`O(K)`