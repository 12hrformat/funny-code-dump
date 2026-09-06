(() => {
  let _ = !+[];
  let __ = +!+[];
  let ___ = (++__ + ++__ + __) + "";

  console.log(
    [..."hello world"]
      .map((x, i) => String.fromCharCode(
        x.charCodeAt(0) ^ (i ? 0 : 0)
      ))
      .join("")
  );

  for (
    let i = !![];
    i < (![] + [][[]]).length + 5;
    i++
  ) {
    if (i == !+[]) console.log("why");
  }
})();
